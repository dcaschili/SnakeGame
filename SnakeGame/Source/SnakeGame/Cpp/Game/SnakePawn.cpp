#include "Game/SnakePawn.h"

#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SnakeLog.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "TimerManager.h"
#include "SnakeGameInstance.h"
#include "Data/GameConstants.h"
#include "Game/EndGameCollisionDetectionComponent.h"
#include "Game/Map/MapOccupancyComponent.h"
#include "Game/Map/MapFunctionLibrary.h"
#include "Game/SnakeBodyPartMoveComponent.h"
#include "Game/SnakeBodyPart.h"


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
			OnChangeDirection.Broadcast(ChangeDirectionAction);
		}
	}
		
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

	const USnakeGameInstance* const GameInstance = Cast<USnakeGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(GameInstance);
	const UGameConstants* const GameConstants = GameInstance->GetGameConstants();
	check(GameConstants);
	TileSize = GameConstants->TileSize;
	HalfTileSize = FMath::RoundToInt32(TileSize / 2.0f);

	FIntVector2 TilePosition{};
	const FVector SnakeLocation = GetActorLocation();
	UMapFunctionLibrary::GetMapTileFromWorldLocation(this, SnakeLocation, TilePosition);
	// Y -> Row; X -> Column
	TilePosition.X--;
	FVector SpawnLocation{};
	UMapFunctionLibrary::GetWorldLocationFromTile(this, TilePosition, SpawnLocation);

	SpawnLocation.Z = GetActorLocation().Z;

	if (ensure(SnakeBodyPartClass))
	{
		ASnakeBodyPart* const SnakeBodyPart = GetWorld()->SpawnActor<ASnakeBodyPart>(SnakeBodyPartClass, SpawnLocation, GetActorRotation());
		SnakeBodyPart->SetSnakePawn(this);
	}
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

