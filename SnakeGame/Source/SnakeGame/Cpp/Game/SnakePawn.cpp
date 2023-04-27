#include "Game/SnakePawn.h"

#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SnakeLog.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "Game/EndGameCollisionDetectionComponent.h"
#include "Game/Map/MapOccupancyComponent.h"
#include "Game/Map/MapFunctionLibrary.h"
#include "Game/SnakeBodyPartMoveComponent.h"
#include "Game/SnakeBodyPart.h"
#include "Game/SnakeBodyPartSpawner.h"
#include "Game/CollectiblesSpawner.h"
#include "SnakeGameGameModeBase.h"

#include "TimerManager.h"

#if !UE_BUILD_SHIPPING
#include "DrawDebugHelpers.h"
#include "Utils/GDTCDebugFunctionLibrary.h"

static TAutoConsoleVariable<bool> CVarSnakePositionDebug(
	TEXT("Snake.EnableSnakePositionDebug"),
	false,
	TEXT("Enable the snake position debugger:\n")
	TEXT("true: draw a sphere on the previous position.\n")
	TEXT("false: disable debugger.\n"),
	ECVF_Cheat);
#endif // !UE_BUILD_SHIPPING

ASnakePawn::ASnakePawn()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	RootComponent = StaticMeshComp;
	StaticMeshComp->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	StaticMeshComp->CastShadow = false;
	StaticMeshComp->SetGenerateOverlapEvents(true);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	if (SpringArmComp)
	{
		SpringArmComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		SpringArmComp->bInheritPitch = false;
		SpringArmComp->bInheritYaw = false;
		SpringArmComp->bInheritRoll = false;
		SpringArmComp->TargetArmLength = 800.0f;
		// Needed to reduce the abrupt change of direction due to the "snap to tile" movement
		// TODO: Do i need it with fixed camera?
		// SpringArmComp->bEnableCameraLag = true;
	}

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	if (CameraComp)
	{
		CameraComp->AttachToComponent(SpringArmComp, FAttachmentTransformRules::KeepRelativeTransform);
	}

	EndGameCollisionComponent = CreateDefaultSubobject<UEndGameCollisionDetectionComponent>(TEXT("EndGameCollisionDetectionComponent"));
	MapOccupancyComponent = CreateDefaultSubobject<UMapOccupancyComponent>(TEXT("MapOccupancyComponent"));
	if (ensure(MapOccupancyComponent))
	{
		MapOccupancyComponent->SetEnableContinuousTileOccupancyTest(true);
	}
	SnakeMovementComponent = CreateDefaultSubobject<USnakeBodyPartMoveComponent>(TEXT("SnakeMovementComponent"));
}

void ASnakePawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

#if !UE_BUILD_SHIPPING
	if (CVarSnakePositionDebug.GetValueOnGameThread())
	{
		// Check if a timer is already active.
		if (!SnakePositionDebuggerTimerHandle.IsValid())
		{
			GetWorldTimerManager().SetTimer(SnakePositionDebuggerTimerHandle, [this]() {
				FVector CurrentPos = GetActorLocation();
				CurrentPos.Z = 0.0f;
				DrawDebugSphere(GetWorld(), CurrentPos, 50.0f, 32, FColor::Red, false, 2.0f);
				}, 0.05f, true);
		}
	}
	else
	{
		// Disable draw debug if it is active.
		if (SnakePositionDebuggerTimerHandle.IsValid())
		{
			GetWorldTimerManager().ClearTimer(SnakePositionDebuggerTimerHandle);
		}
	}
#endif // !UE_BUILD_SHIPPING

	FVector CurrentPos = GetActorLocation();

	if (PendingMoveDirection.IsSet())
	{
		// Check if the snake is near the center of the tile to allow for the change in direction
		// If this is basically the center of the tile, allow the change in direction. 
		// Otherwise let's proceed in this same direction until we reach a center.	
		if(UMapFunctionLibrary::IsWorldLocationNearCurrentTileCenter(this, CurrentPos))
		{ 
			MoveDirection = PendingMoveDirection.GetValue();
			PendingMoveDirection.Reset();

			check(SnakeMovementComponent);
			SnakeMovementComponent->ChangeMoveDirection(MoveDirection);

			FChangeDirectionAction ChangeDirectionAction{};
			ChangeDirectionAction.Direction = MoveDirection;
			ChangeDirectionAction.Location = CurrentPos;

			for (ASnakeBodyPart* const BodyPart : SnakeBody)
			{
				if (ensure(BodyPart))
				{
					BodyPart->AddChangeDirAction(ChangeDirectionAction);
				}
			}

			OnChangeDirection.Broadcast(ChangeDirectionAction);
		}
	}
}

void ASnakePawn::AddSnakeBodyPart(ASnakeBodyPart* InSnakeBodyPart)
{
	if (InSnakeBodyPart)
	{
		SnakeBody.Add(InSnakeBodyPart);
		UE_LOG(SnakeLogCategorySnakeBody, Verbose, TEXT("Added new snake body part!"));
	}
}

FVector ASnakePawn::GetMoveDirection() const
{
	if (ensure(SnakeMovementComponent))
	{
		return SnakeMovementComponent->GetMoveDirection();
	}
	return FVector::RightVector;
}

void ASnakePawn::BeginPlay()
{
	Super::BeginPlay();

	// Setup input
	if (InputMappingContext)
	{
		APlayerController* PC = Cast<APlayerController>(Controller);
		ULocalPlayer* LocalPlayer = PC ? PC->GetLocalPlayer() : nullptr;
		if (LocalPlayer)
		{
			UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
			if (EnhancedInputSubsystem)
			{
				EnhancedInputSubsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
	else
	{
		UE_LOG(SnakeLogCategoryGame, Warning, TEXT("Missing InputMapping Context"));
		ensure(false);
	}

	// Spawn initial body part spawner
	if (ensure(SnakeBodyPartSpawnerClass))
	{
		FVector SpawnLocation{};
		if (ensure(UMapFunctionLibrary::AlignWorldLocationToTileCenter(this, GetActorLocation(), SpawnLocation)))
		{
			UWorld* const World = GetWorld();
			if (ensure(World))
			{
				SpawnLocation.Z = GetActorLocation().Z;
				ASnakeBodyPartSpawner* BodyPartSpawner = World->SpawnActor<ASnakeBodyPartSpawner>(SnakeBodyPartSpawnerClass, SpawnLocation, FRotator::ZeroRotator);
				if (ensure(BodyPartSpawner) && InitialBodyPartsCount > 1)
				{
					BodyPartSpawner->SetBodyPartToSpawnCount(InitialBodyPartsCount);
				}
			}
		}
	}

	BindEvents();
}

void ASnakePawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindEvents();

	Super::EndPlay(EndPlayReason);
}

void ASnakePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (ensure(EnhancedInputComponent))
	{
		if (MoveRightIA)
		{
			EnhancedInputComponent->BindAction(MoveRightIA, ETriggerEvent::Triggered, this, &ThisClass::HandleMoveRightIA);
		}
		else
		{
			UE_LOG(SnakeLogCategoryGame, Warning, TEXT("Missing MoveRightIA!"));
			ensure(false);
		}
		
		if (MoveUpIA)
		{
			EnhancedInputComponent->BindAction(MoveUpIA, ETriggerEvent::Triggered, this, &ThisClass::HandleMoveUpIA);
		}
		else
		{
			UE_LOG(SnakeLogCategoryGame, Warning, TEXT("Missing MoveUpIA!"));
			ensure(false);
		}
	}
}

void ASnakePawn::BindEvents()
{
	if (ASnakeGameGameModeBase* GameMode = Cast<ASnakeGameGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		if (ACollectiblesSpawner* const Spawner = GameMode->GetCollectiblesSpawner())
		{
			Spawner->OnCollectibleCollected.AddUniqueDynamic(this, &ThisClass::HandleCollectibleCollected);
		}
	}
}

void ASnakePawn::UnbindEvents()
{
	if (ASnakeGameGameModeBase* GameMode = Cast<ASnakeGameGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		if (ACollectiblesSpawner* const Spawner = GameMode->GetCollectiblesSpawner())
		{
			Spawner->OnCollectibleCollected.RemoveDynamic(this, &ThisClass::HandleCollectibleCollected);
		}
	}
}

void ASnakePawn::HandleCollectibleCollected(const FVector& InCollectibleLocation)
{
	UE_LOG(SnakeLogCategorySnakeBody, Verbose, TEXT("ASnakePawn - Spawning body part spawner!"));
	
	if (ensure(SnakeBodyPartSpawnerClass))
	{
		check(GetWorld());
		GetWorld()->SpawnActor<ASnakeBodyPartSpawner>(SnakeBodyPartSpawnerClass, InCollectibleLocation, FQuat::Identity.Rotator());
	}
}

void ASnakePawn::HandleMoveRightIA(const FInputActionInstance& InputActionInstance)
{
	if (InputActionInstance.GetValue().IsNonZero())
	{
		// Can't change direction left/right without first going up or down.
		if (!PendingMoveDirection.IsSet() && FMath::IsNearlyZero(MoveDirection.Y))
		{
			const float Amount = InputActionInstance.GetValue().Get<float>();
			PendingMoveDirection = FVector(0.0f, Amount, 0.0f);
		}
	}
}

void ASnakePawn::HandleMoveUpIA(const FInputActionInstance& InputActionInstance)
{
	if (InputActionInstance.GetValue().IsNonZero())
	{
		// Can't change direction up/down without first going left or right.
		if (!PendingMoveDirection.IsSet() && FMath::IsNearlyZero(MoveDirection.X))
		{
			const float Amount = InputActionInstance.GetValue().Get<float>();
			PendingMoveDirection = FVector(Amount, 0.0f, 0.0f);
		}
	}
}

