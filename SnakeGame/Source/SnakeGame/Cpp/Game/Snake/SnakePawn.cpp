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
#include "Game/Map/MapOccupancyComponent.h"
#include "Game/Components/EndGameOverlapDetectionComponent.h"
#include "Game/CollectiblesSpawner.h"
#include "SnakeGameGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SplineComponent.h"

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

static ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::World;

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
	EndGameOverlapComponent = CreateDefaultSubobject<UEndGameOverlapDetectionComponent>(TEXT("EndGameOverlapDetectionComponent"));
	MapOccupancyComponent = CreateDefaultSubobject<UMapOccupancyComponent>(TEXT("MapOccupancyComponent"));
	if (ensure(MapOccupancyComponent))
	{
		MapOccupancyComponent->SetEnableContinuousTileOccupancyTest(true);
	}

	SnakeBodySplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SnakeBodySplineComponent"));
	if (ensure(SnakeBodySplineComponent))
	{
		//SnakeBodySplineComponent->SetupAttachment(RootComponent);
		SnakeBodySplineComponent->ClearSplinePoints(false);
	}
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

	SnakeBodySplineComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

	GDTUI_SHORT_LOG(SnakeLogCategorySnakeBody, Log, TEXT("Added snake head spline point!"));
	AddSplinePointAtLocation(GetActorLocation());
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

			OnChangeDirection.Broadcast(ChangeDirectionAction);
		}
	}

	if (!bChangeDirectionEnabled && !bIsPositionNearTileCenter)
	{
		GDTUI_SHORT_LOG(SnakeLogCategorySnakeBody, VeryVerbose, TEXT("Enable change direction!"));
		bChangeDirectionEnabled = true;
	}

	if (SnakeBodySplineComponent)
	{
		UpdateSplinePoints();
	}
}

const ASnakeBodyPart* ASnakePawn::GetSnakeBodyPartAtIndex(int32 InBodyPartIndex) const
{
	if (SnakeBody.IsValidIndex(InBodyPartIndex))
	{
		return SnakeBody[InBodyPartIndex];
	}
	return nullptr;
}

void ASnakePawn::AddSnakeBodyPart(ASnakeBodyPart* InSnakeBodyPart)
{
	if (InSnakeBodyPart)
	{
		SnakeBody.Add(InSnakeBodyPart);
		InSnakeBodyPart->SetSnakeBodyPartIndex(SnakeBody.Num() - 1);
		GDTUI_SHORT_LOG(SnakeLogCategorySnakeBody, Verbose, TEXT("Added new snake body part!"));
		
		AddSplinePointAtLocation(InSnakeBodyPart->GetActorLocation());
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

//TOptional<FVector> ASnakePawn::GetBodyPartSplinePointPosition(int32 InBodyPartIndex) const
//{
//	TOptional<FVector> Pos{};
//
//	if (!SnakeBodySplineComponent)
//	{
//		GDTUI_LOG(SnakeLogCategorySnakeBody, Error, TEXT("Missing spline component!"));
//		ensure(false);
//		return Pos;
//	}
//
//	if (SnakeBody.IsValidIndex(InBodyPartIndex))
//	{
//		const int32 BodyPartSplineIndex = InBodyPartIndex++;
//		const int32 SplinePointsCount = SnakeBodySplineComponent->GetNumberOfSplinePoints();
//		if (BodyPartSplineIndex >= 0 && BodyPartSplineIndex < SplinePointsCount)
//		{
//			Pos = SnakeBodySplineComponent->GetLocationAtSplinePoint(BodyPartSplineIndex, ESplineCoordinateSpace::World);			
//		}
//	}
//	else
//	{
//		GDTUI_LOG(SnakeLogCategorySnakeBody, Warning, TEXT("Index %d isn't a valid body part index!"), InBodyPartIndex);
//		ensure(false);
//	}
//
//	return Pos;
//}
//
//TOptional<FVector> ASnakePawn::GetBodyPartSplinePointTangent(int32 InBodyPartIndex) const
//{
//	TOptional<FVector> Tan{};
//
//	if (!SnakeBodySplineComponent)
//	{
//		GDTUI_LOG(SnakeLogCategorySnakeBody, Error, TEXT("Missing spline component!"));
//		ensure(false);
//		return Tan;
//	}
//
//	if (SnakeBody.IsValidIndex(InBodyPartIndex))
//	{
//		const int32 BodyPartSplineIndex = InBodyPartIndex++;
//		const int32 SplinePointsCount = SnakeBodySplineComponent->GetNumberOfSplinePoints();
//		if (BodyPartSplineIndex >= 0 && BodyPartSplineIndex < SplinePointsCount)
//		{
//			Tan = SnakeBodySplineComponent->GetTangentAtSplinePoint(BodyPartSplineIndex, ESplineCoordinateSpace::World);
//		}
//	}
//	else
//	{
//		GDTUI_LOG(SnakeLogCategorySnakeBody, Warning, TEXT("Index %d isn't a valid body part index!"), InBodyPartIndex);
//		ensure(false);
//	}
//
//	return Tan;
//}

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
	GDTUI_SHORT_LOG(SnakeLogCategorySnakeBody, Verbose, TEXT("Spawning body part spawner!"));

	if (ensure(SnakeBodyPartSpawnerClass))
	{
		UWorld* World = GetWorld();
		if (ensure(World))
		{
			World->SpawnActor<ASnakeBodyPartSpawner>(SnakeBodyPartSpawnerClass, InCollectibleLocation, FRotator::ZeroRotator);
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

void ASnakePawn::UpdateSplinePoints()
{
	check(SnakeBodySplineComponent);
	
	const int32 SplinePointsCount = SnakeBodySplineComponent->GetNumberOfSplinePoints();
	for (int32 i = 0; i < SplinePointsCount; ++i)
	{
		if (i == 0)
		{
			// Update head
			UpdateSplinePointLocation(i, GetActorLocation());
		}
		else
		{
			const int32 BodyPartIndex = i - 1;
			if (SnakeBody.IsValidIndex(BodyPartIndex))
			{
				if (const ASnakeBodyPart* const BodyPart = SnakeBody[BodyPartIndex])
				{
					UpdateSplinePointLocation(i, BodyPart->GetActorLocation());
				}
			}
			else
			{
				GDTUI_LOG(SnakeLogCategorySnakeBody, Error, TEXT("Misalignment between spline points and snake body parts array!"));
				ensure(false);
				return;
			}
		}
	}
}

void ASnakePawn::UpdateSplinePointLocation(int32 Index, const FVector& InLocation)
{
	check(SnakeBodySplineComponent);
	if (Index < 0)
	{
		GDTUI_LOG(SnakeLogCategorySnakeBody, Warning, TEXT("Requested negative spline point index to update!"));
		ensure(false);
	}
	else
	{
		SnakeBodySplineComponent->SetLocationAtSplinePoint(Index, InLocation, CoordSpace);
	}
}

void ASnakePawn::AddSplinePointAtLocation(const FVector& InPosition)
{
	if (SnakeBodySplineComponent)
	{
		GDTUI_SHORT_LOG(SnakeLogCategorySnakeBody, Verbose, TEXT("Added spline point at position: %s"), *InPosition.ToString());
		SnakeBodySplineComponent->AddSplinePoint(InPosition, CoordSpace);
	}
	else
	{
		GDTUI_LOG(SnakeLogCategorySnakeBody, Error, TEXT("Missing Snake body spline component! Can't add spline point!"));
		ensure(false);		
	}
}

