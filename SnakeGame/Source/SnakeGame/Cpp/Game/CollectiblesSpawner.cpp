#include "Game/CollectiblesSpawner.h"

#include "Kismet/GameplayStatics.h"
#include "Game/CollectibleActor.h"
#include "Engine/World.h"
#include "SnakeLog.h"
#include "SnakeMatchGameModeBase.h"
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
	
	if (ASnakeMatchGameModeBase* const GameModeBase = Cast<ASnakeMatchGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		GameModeBase->OnMatchStarted.AddUniqueDynamic(this, &ThisClass::HandleOnMatchStarted);
	}
}

void ACollectiblesSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (ACollectibleActor* const Collectible : CollectibleActorsPoll)
	{
		if (Collectible)
		{
			Collectible->OnCollectedActor.RemoveDynamic(this, &ThisClass::HandleCollectibleCollected);
		}
	}

	if (ASnakeMatchGameModeBase* const GameModeBase = Cast<ASnakeMatchGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		GameModeBase->OnMatchStarted.RemoveDynamic(this, &ThisClass::HandleOnMatchStarted);
	}

	Super::EndPlay(EndPlayReason);
}

void ACollectiblesSpawner::HandleOnMatchStarted()
{
	GDTUI_SHORT_LOG(SnakeLogCategorySpawner, Log, TEXT("Match started, start spawning collectibles!"));
	SpawnCollectible();
}

void ACollectiblesSpawner::HandleCollectibleCollected(const FVector& InCollectibleLocation)
{
	if (ensure(ActiveCollectible.IsSet()))
	{
		OnCollectibleCollected.Broadcast(InCollectibleLocation);

		// Change position
		AMapManager* const MapManager = AMapManager::GetMapManager(this);
		if (ensure(MapManager))
		{
			FVector SpawnLocation{};
			if (ensure(MapManager->GetRandomFreeMapLocation(SpawnLocation)))
			{
				/*
					Ensures that the previous position can't be used as new location
				*/
				if (LastSpawnLocation.IsSet())
				{
					const FVector PreviousCollectibleLocation = LastSpawnLocation.GetValue();

					while ((SpawnLocation - PreviousCollectibleLocation).IsNearlyZero())
					{
						UE_LOG(SnakeLogCategorySpawner, Verbose, TEXT("Generated a new collectible position equals to the previous one, continuing generating until a different position is obtained!"));
						ensure(MapManager->GetRandomFreeMapLocation(SpawnLocation));
					}
				}

				SpawnLocation.Z = SpawningStartingHeight;
				UE_LOG(SnakeLogCategorySpawner, Verbose, TEXT("Spawned collectible at position %s"), *SpawnLocation.ToString());

				// Move collectible out of map
				ACollectibleActor* const ToDisableCollectible = CollectibleActorsPoll.IsValidIndex(ActiveCollectible.GetValue()) ? CollectibleActorsPoll[ActiveCollectible.GetValue()] : nullptr;
				if (ToDisableCollectible)
				{
					ToDisableCollectible->SetActorLocation(OutOfMapLocation);						
				}
				
				// Activate the new collectible
				ActiveCollectible = (ActiveCollectible.GetValue() + 1) % CollectibleActorsPoll.Num();
				ACollectibleActor* const ToSpawnCollectible = CollectibleActorsPoll.IsValidIndex(ActiveCollectible.GetValue()) ? CollectibleActorsPoll[ActiveCollectible.GetValue()] : nullptr;
				if (ToSpawnCollectible)
				{
					ToSpawnCollectible->EnableCollectible();
					ToSpawnCollectible->SetActorLocation(SpawnLocation, false);
				}
			}
		}
	}
}

void ACollectiblesSpawner::InitializeCollectiblePool()
{
	if (CollectibleClass)
	{
		CollectibleActorsPoll.Empty();
		if (UWorld* const World = GetWorld())
		{
			for (int32 i = 0; i < CollectiblePoolSize; ++i)
			{
				ACollectibleActor* const Collectible = World->SpawnActor<ACollectibleActor>(CollectibleClass, OutOfMapLocation, FRotator::ZeroRotator);
				if (ensure(Collectible))
				{
					Collectible->DisableCollectible();				
					Collectible->OnCollectedActor.AddUniqueDynamic(this, &ThisClass::HandleCollectibleCollected);
					
					CollectibleActorsPoll.Add(Collectible);
				}
			}
		}
	}
	else
	{
		GDTUI_LOG(SnakeLogCategorySpawner, Error, TEXT("Missing collectible class!"));
		ensure(false);
	}
}

void ACollectiblesSpawner::SpawnCollectible()
{
	if (CollectibleActorsPoll.IsEmpty())
	{
		InitializeCollectiblePool();
	}

	if (ensure(!CollectibleActorsPoll.IsEmpty()))
	{
		AMapManager* const MapManager = AMapManager::GetMapManager(this);
		if (ensure(MapManager))
		{
			FVector SpawnLocation{};
			if (ensure(MapManager->GetRandomFreeMapLocation(SpawnLocation)))
			{
				/*
					Ensures that the previous position can't be used as new location
				*/
				if (LastSpawnLocation.IsSet())
				{
					const FVector PreviousCollectibleLocation = LastSpawnLocation.GetValue();

					while ((SpawnLocation - PreviousCollectibleLocation).IsNearlyZero())
					{
						GDTUI_LOG(SnakeLogCategorySpawner, Verbose, TEXT("Generated a new collectible position equals to the previous one, continuing generating until a different position is obtained!"));
						ensure(MapManager->GetRandomFreeMapLocation(SpawnLocation));
					}
				}

				SpawnLocation.Z = SpawningStartingHeight;
				GDTUI_SHORT_LOG(SnakeLogCategorySpawner, Verbose, TEXT("Spawned collectible at position %s"), *SpawnLocation.ToString());
				
				// Start by the first in the array
				ActiveCollectible = 0;

				ACollectibleActor* const CollectibleActor = CollectibleActorsPoll[ActiveCollectible.GetValue()];
				CollectibleActor->EnableCollectible();
				CollectibleActor->SetActorLocation(SpawnLocation, false);

				CollectibleActor->OnCollectedActor.AddUniqueDynamic(this, &ThisClass::HandleCollectibleCollected);

				LastSpawnLocation = SpawnLocation;
			}
		}
	}
}
