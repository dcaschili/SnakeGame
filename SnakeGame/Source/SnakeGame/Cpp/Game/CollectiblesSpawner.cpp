#include "Game/CollectiblesSpawner.h"

#include "Kismet/GameplayStatics.h"
#include "Game/CollectibleActor.h"
#include "Engine/World.h"
#include "SnakeLog.h"
#include "SnakeGameGameModeBase.h"
#include "Game/Map/MapManager.h"

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

void ACollectiblesSpawner::HandleCollectibleCollected()
{
	if (ActiveCollectibleActor.IsValid())
	{
		ActiveCollectibleActor->OnCollectedActor.RemoveDynamic(this, &ThisClass::HandleCollectibleCollected);
	}
	SpawnCollectible();
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
				/*
					Ensures that the previous position can't be used as new location
				*/
				if (ActiveCollectibleActor.IsValid())
				{
					const FVector PreviousCollectibleLocation = ActiveCollectibleActor->GetActorLocation();

					while ((SpawnLocation - PreviousCollectibleLocation).IsNearlyZero())
					{
						UE_LOG(SnakeLogCategorySpawner, Verbose, TEXT("Generated a new collectible position equals to the previous one, continuing generating until a different position is obtained!"));
						ensure(MapManager->GetRandomFreeMapLocation(SpawnLocation));
					}
				}

				SpawnLocation.Z = SpawningStartingHeight;
				UE_LOG(SnakeLogCategorySpawner, Verbose, TEXT("Spawned collectible at position %s"), *SpawnLocation.ToString());

				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;


				ActiveCollectibleActor = GetWorld()->SpawnActor<ACollectibleActor>(CollectibleClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
				if (ensure(ActiveCollectibleActor.IsValid()))
				{
					ActiveCollectibleActor->OnCollectedActor.AddUniqueDynamic(this, &ThisClass::HandleCollectibleCollected);
				}
			}
		}
	}
}
