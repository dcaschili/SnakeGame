#include "Game/Snake/SnakePawn.h"

#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "SnakeLog.h"
#include "Game/Map/MapFunctionLibrary.h"
#include "Game/Components/SnakeBodyPartMoveComponent.h"
#include "Game/Snake/SnakeBodyPart.h"
#include "Game/Snake/SnakeBodyPartSpawner.h"
#include "Audio/SnakeChangeDirectionAudioComponent.h"
#include "Engine/World.h"

#if !UE_BUILD_SHIPPING
#include "DrawDebugHelpers.h"
#include "Utils/GDTCDebugFunctionLibrary.h"
#include "TimerManager.h"

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
	
	SnakeMovementComponent = CreateDefaultSubobject<USnakeBodyPartMoveComponent>(TEXT("SnakeMovementComponent"));
	SnakeChangeDirectionAudioComponent = CreateDefaultSubobject<USnakeChangeDirectionAudioComponent>(TEXT("SnakeChangeDirectionAudioComponent"));
}

void ASnakePawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

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

	const bool bIsPositionNearTileCenter = UMapFunctionLibrary::IsWorldLocationNearCurrentTileCenter(this, CurrentPos);

	if (PendingMoveDirection.IsSet() && bChangeDirectionEnabled)
	{
		// Check if the snake is near the center of the tile to allow for the change in direction
		// If this is basically the center of the tile, allow the change in direction. 
		// Otherwise let's proceed in this same direction until we reach a center.	
		if(bIsPositionNearTileCenter)
		{ 
			MoveDirection = PendingMoveDirection.GetValue();
			PendingMoveDirection.Reset();
			GDTUI_SHORT_LOG(SnakeLogCategorySnakeBody, VeryVerbose, TEXT("Disable change direction!"));
			bChangeDirectionEnabled = false;

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

	if (!bChangeDirectionEnabled && !bIsPositionNearTileCenter)
	{
		GDTUI_SHORT_LOG(SnakeLogCategorySnakeBody, VeryVerbose, TEXT("Enable change direction!"));
		bChangeDirectionEnabled = true;
	}
}

void ASnakePawn::AddSnakeBodyPart(ASnakeBodyPart* InSnakeBodyPart)
{
	if (InSnakeBodyPart)
	{
		SnakeBody.Add(InSnakeBodyPart);
		GDTUI_SHORT_LOG(SnakeLogCategorySnakeBody, Verbose, TEXT("Added new snake body part!"));
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
			GDTUI_LOG(SnakeLogCategoryGame, Warning, TEXT("Missing MoveRightIA!"));
			ensure(false);
		}
		
		if (MoveUpIA)
		{
			EnhancedInputComponent->BindAction(MoveUpIA, ETriggerEvent::Triggered, this, &ThisClass::HandleMoveUpIA);
		}
		else
		{
			GDTUI_LOG(SnakeLogCategoryGame, Warning, TEXT("Missing MoveUpIA!"));
			ensure(false);
		}
	}
}

TOptional<FChangeDirectionAction> ASnakePawn::BuildChangeDirectionAction() const
{
	return TOptional<FChangeDirectionAction>();
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

