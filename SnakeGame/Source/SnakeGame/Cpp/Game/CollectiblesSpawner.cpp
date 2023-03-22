#include "Game/CollectiblesSpawner.h"

#include "Kismet/GameplayStatics.h"
#include "Game/CollectibleActor.h"
#include "Engine/World.h"
#include "SnakeLog.h"
#include "SnakeGameGameModeBase.h"
#include "Game/MapManager.h"

ACollectiblesSpawner::ACollectiblesSpawner()
	: Super()
	, CollectibleClass{ ACollectibleActor::StaticClass() }
{
	check(IsValid(CollectibleClass));
}

void ACollectiblesSpawner::BeginPlay()
{
	Super::BeginPlay();
	SpawnCollectible();
}

void ACollectiblesSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ACollectiblesSpawner::HandleCollectibleDestroyed()
{
}

void ACollectiblesSpawner::SpawnCollectible()
{
	if (GetWorld())
	{
		check(IsValid(CollectibleClass));

		AMapManager* const MapManager = AMapManager::GetMapManager(this);
		if (ensure(MapManager))
		{
			FVector SpawnLocation{};
			if (ensure(MapManager->GetRandomFreeMapLocation(SpawnLocation)))
			{
				SpawnLocation.Z = SpawningStartingHeight;
				UE_LOG(SnakeLogCategorySpawner, Verbose, TEXT("Spawned collectible at position %s"), *SpawnLocation.ToString());

				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;


				ActiveCollectibleActor = GetWorld()->SpawnActor<ACollectibleActor>(CollectibleClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			}

		}
	}
}
