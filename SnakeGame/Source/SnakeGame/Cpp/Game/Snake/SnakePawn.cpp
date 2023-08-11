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
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Game/ChangeDirectionAction.h"
#include "Game/GrassTrailSceneCaptureActor.h"


ASnakePawn::ASnakePawn()
{
	PrimaryActorTick.bCanEverTick = false;

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

	SnakeTrailNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SnakeTrailNiagaraComponent"));
	SnakeTrailNiagaraComponent->SetupAttachment(RootComponent);	
}

void ASnakePawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!NewController) return;

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

	SpawnTrailCaptureActor();

	/*
		This is needed so that the bOwnerNoSee in the Niagara trail particle
		system works for this pawn.	
	*/
	SetOwner(NewController->GetViewTarget());	
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
	if (SnakeMovementComponent && SnakeMovementComponent->IsChangeDirActionPending())
	{
		GDTUI_SHORT_LOG(SnakeLogCategoryInput, Verbose, TEXT("Can't change direction, there is still an action pending!"));
		return;
	}

	if (InputActionInstance.GetValue().IsNonZero())
	{
		// Can't change direction left/right without first going up or down.
		if (FMath::IsNearlyZero(GetMoveDirection().Y))
		{
			const float Amount = InputActionInstance.GetValue().Get<float>();
			const FVector NewDir = FVector(0.0f, Amount, 0.0f);
			PerformChangeDir(NewDir);
		}
	}
}

void ASnakePawn::HandleMoveUpIA(const FInputActionInstance& InputActionInstance)
{
	if (SnakeMovementComponent && SnakeMovementComponent->IsChangeDirActionPending())
	{
		GDTUI_SHORT_LOG(SnakeLogCategoryInput, Verbose, TEXT("Can't change direction, there is still an action pending!"));
		return;
	}

	if (InputActionInstance.GetValue().IsNonZero())
	{
		// Can't change direction up/down without first going left or right.
		if (FMath::IsNearlyZero(GetMoveDirection().X))
		{
			const float Amount = InputActionInstance.GetValue().Get<float>();
			const FVector NewDir = FVector(Amount, 0.0f, 0.0f);
			PerformChangeDir(NewDir);
		}
	}
}

void ASnakePawn::ExtendSnakeBody()
{
	if (!SnakeBodyNiagaraParamCollection)
	{
		GDTUI_PRINT_TO_SCREEN_ERROR(TEXT("ERROR! Missing niagara param collection reference in snake pawn!"));
		return;
	}

	if (UNiagaraParameterCollectionInstance* const SnakeBodyNiagaraParmCollectionInstance = UNiagaraFunctionLibrary::GetNiagaraParameterCollection(this, SnakeBodyNiagaraParamCollection))
	{		
		GDTUI_LOG(SnakeLogCategorySnakeBody, Verbose, TEXT("Snake body extended!"));
		SnakeBodyNiagaraParmCollectionInstance->SetFloatParameter(TotBodyCountNiagaraParamName, SnakeBody.Num());
	}
	else
	{
		GDTUI_LOG(SnakeLogCategorySnakeBody, Warning, TEXT("Can't find niagara parameter collection instance!"));
		ensure(false);
	}
}

FVector ASnakePawn::GenerateChangeDirectionActionLocation() const
{
	FVector ChangeDirTileCenter{};
	if (ensure(SnakeMovementComponent))
	{
		const FVector MoveDir = SnakeMovementComponent->GetMoveDirection();

		const FVector CurrentPos = GetActorLocation();
		FVector NearestTileCenter{};

		// Get nearest tile center to current position
		if (UMapFunctionLibrary::AlignWorldLocationToTileCenter(this, CurrentPos, NearestTileCenter))
		{
			// Check if we are beyond the nearest tile center
			const bool bOvershoot = UMapFunctionLibrary::DoesOvershootPosition(this, CurrentPos, MoveDir, NearestTileCenter);
			if (bOvershoot)
			{
				// Given that we are beyond the nearest tile center, use the next to change direction.
				FVector NextTileToChangeDir{};
				if (UMapFunctionLibrary::GetFollowingTile(this, NearestTileCenter, MoveDir, NextTileToChangeDir))
				{
					ChangeDirTileCenter = NextTileToChangeDir;
				}	
				else
				{
					GDTUI_LOG(SnakeLogCategoryMap, Warning, TEXT("Something went wrong in finding the next tile in the map!"));
					ensure(false);
				}
			}
			else
			{
				// We still have to reach the nearest tile center
				ChangeDirTileCenter = NearestTileCenter;
			}			
		}
		else
		{
			GDTUI_LOG(SnakeLogCategoryMap, Warning, TEXT("Can't convert world location to tile center!"));
			ensure(false);
		}
	}

	ChangeDirTileCenter.Z = GetActorLocation().Z;
	return ChangeDirTileCenter;
}

void ASnakePawn::PerformChangeDir(const FVector& InNewDir)
{
	if (ensure(SnakeMovementComponent))
	{
		FChangeDirectionAction ChangeDirAction{};
		ChangeDirAction.Direction = InNewDir;
		ChangeDirAction.Location = GenerateChangeDirectionActionLocation();

		SnakeMovementComponent->AddChangeDirAction(ChangeDirAction);

		OnChangeDirection.Broadcast(ChangeDirAction);
	}
}

void ASnakePawn::SpawnTrailCaptureActor()
{
	if (TrailCaptureActorTag.IsNone())
	{
		GDTUI_PRINT_TO_SCREEN_ERROR(TEXT("Missing tag to get trail capture actor transform!"));
		ensure(false);
		return;
	}

	if (TrailCaptureActorClass)
	{

		TArray<AActor*> FoundActors{};
		UGameplayStatics::GetAllActorsWithTag(this, TrailCaptureActorTag, FoundActors);

		if (FoundActors.IsEmpty())
		{
			GDTUI_PRINT_TO_SCREEN_ERROR(TEXT("Missing actor with tag %s. It is needed to spawn trail capture actor!"), *TrailCaptureActorTag.ToString());
			ensure(false);
			return;
		}

		if (FoundActors.Num() > 1)
		{
			GDTUI_PRINT_TO_SCREEN_WARN(TEXT("Found more than one actor with tag: %s. Just the first one will be used!"), *TrailCaptureActorTag.ToString());
		}

		if (AActor* const Actor = FoundActors[0])
		{
			if (UWorld* const World = GetWorld())
			{
				GrassTrailCaptureActor = World->SpawnActor<AGrassTrailSceneCaptureActor>(TrailCaptureActorClass, Actor->GetTransform());
			}
		}
	}
	else
	{
		GDTUI_PRINT_TO_SCREEN_ERROR(TEXT("Missing trail capture actor class!"));
		ensure(false);
	}
}

