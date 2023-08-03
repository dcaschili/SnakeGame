#include "Game/Snake/SnakePawn.h"

#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "SnakeLog.h"
#include "Game/Map/MapFunctionLibrary.h"
#include "Game/Components/SnakeMoveComponent.h"
#include "Game/Snake/SnakeBodyPart.h"
#include "Game/Snake/SnakeBodyPartSpawner.h"
#include "Audio/SnakeChangeDirectionAudioComponent.h"
#include "Engine/World.h"
#include "Game/Map/MapOccupancyComponent.h"
#include "Game/Components/EndGameOverlapDetectionComponent.h"
#include "Game/CollectiblesSpawner.h"
#include "SnakeGameGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Game/Snake/SnakeBodySplineManager.h"
#include "NiagaraComponent.h"


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
	
	SnakeMovementComponent = CreateDefaultSubobject<USnakeMoveComponent>(TEXT("SnakeMovementComponent"));
	SnakeChangeDirectionAudioComponent = CreateDefaultSubobject<USnakeChangeDirectionAudioComponent>(TEXT("SnakeChangeDirectionAudioComponent"));
	EndGameOverlapComponent = CreateDefaultSubobject<UEndGameOverlapDetectionComponent>(TEXT("EndGameOverlapDetectionComponent"));
	MapOccupancyComponent = CreateDefaultSubobject<UMapOccupancyComponent>(TEXT("MapOccupancyComponent"));
	if (ensure(MapOccupancyComponent))
	{
		MapOccupancyComponent->SetEnableContinuousTileOccupancyTest(true);
	}

	SnakeBodyRibbonSystemComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SnakeBodyRibbonSystemComponent"));
	SnakeBodyRibbonSystemComponent->SetupAttachment(RootComponent);
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
	else
	{
		GDTUI_PRINT_TO_SCREEN_ERROR(TEXT("Missing Snake body part spawner class!"));
		ensure(false);
	}

	if (NewController && NewController->IsLocalController())
	{
		if (!SnakeBodySplineManager)
		{
			if (SnakeBodySplineManagerClass)
			{
				UWorld* const World = GetWorld();
				if (ensure(World))
				{
					SnakeBodySplineManager = World->SpawnActor<ASnakeBodySplineManager>(SnakeBodySplineManagerClass);
					if (SnakeBodySplineManager)
					{
						SnakeBodySplineManager->SetSnakePawn(this);
					}
				}
			}
			else
			{
				GDTUI_PRINT_TO_SCREEN_ERROR(TEXT("Missing snake spline body manager class!"));
				ensure(false);
			}
		}
		else
		{
			GDTUI_LOG(SnakeLogCategorySnakeBody, Warning, TEXT("Trying to initializa another Snake body spline manager for the same snake!"));
			ensure(false);
		}
	}

}

void ASnakePawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

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
		GDTUI_SHORT_LOG(SnakeLogCategorySnakeBody, Verbose, TEXT("Added new snake body part!"));
		ExtendSnakeBody();
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

void ASnakePawn::ExtendSnakeBody()
{
	check(SnakeBodyRibbonSystemLifetimeIncrementPerBodyPart >= 0.0f);
	check(SnakeBodyRibbonSystemLifetime >= 0);

	if (ensure(SnakeBodyRibbonSystemComponent))
	{
		SnakeBodyRibbonSystemLifetime += SnakeBodyRibbonSystemLifetimeIncrementPerBodyPart;
		SnakeBodyRibbonSystemComponent->SetFloatParameter(SnakeBodyRibbonSystemLifetimeParameterName, SnakeBodyRibbonSystemLifetime);
		GDTUI_PRINT_TO_SCREEN_LOG(TEXT("Extended!"));
	}
}

