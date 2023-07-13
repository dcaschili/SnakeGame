#include "Game/Snake/SnakeMatchPawn.h"

#include "Game/Map/MapOccupancyComponent.h"
#include "Game/Components/EndGameOverlapDetectionComponent.h"
#include "Game/CollectiblesSpawner.h"
#include "SnakeGameGameModeBase.h"
#include "SnakeLog.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Game/Snake/SnakeBodyPartSpawner.h"

ASnakeMatchPawn::ASnakeMatchPawn()
	: Super()
{
	EndGameOverlapComponent = CreateDefaultSubobject<UEndGameOverlapDetectionComponent>(TEXT("EndGameOverlapDetectionComponent"));
	MapOccupancyComponent = CreateDefaultSubobject<UMapOccupancyComponent>(TEXT("MapOccupancyComponent"));
	if (ensure(MapOccupancyComponent))
	{
		MapOccupancyComponent->SetEnableContinuousTileOccupancyTest(true);
	}
}

void ASnakeMatchPawn::BindEvents()
{
	Super::BindEvents();

	if (ASnakeGameGameModeBase* GameMode = Cast<ASnakeGameGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		if (ACollectiblesSpawner* const Spawner = GameMode->GetCollectiblesSpawner())
		{
			Spawner->OnCollectibleCollected.AddUniqueDynamic(this, &ThisClass::HandleCollectibleCollected);
		}
	}
}

void ASnakeMatchPawn::UnbindEvents()
{
	if (ASnakeGameGameModeBase* GameMode = Cast<ASnakeGameGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		if (ACollectiblesSpawner* const Spawner = GameMode->GetCollectiblesSpawner())
		{
			Spawner->OnCollectibleCollected.RemoveDynamic(this, &ThisClass::HandleCollectibleCollected);
		}
	}

	Super::UnbindEvents();
}

void ASnakeMatchPawn::HandleCollectibleCollected(const FVector& InCollectibleLocation)
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
