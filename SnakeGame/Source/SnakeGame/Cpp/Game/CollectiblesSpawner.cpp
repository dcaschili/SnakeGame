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
	if (ActiveCollectibleActor)
	{
		ActiveCollectibleActor->OnCollectedActor.RemoveDynamic(this, &ThisClass::HandleCollectibleCollected);
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
	if (ensure(ActiveCollectibleActor))
	{
		ActiveCollectibleActor->DisableCollectible();
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

				ActiveCollectibleActor->SetActorLocation(SpawnLocation, false);
				ActiveCollectibleActor->EnableCollectible();
			}
		}
	}
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
				
				if (UWorld* const World = GetWorld())
				{
					FActorSpawnParameters SpawnParams;
					SpawnParams.Owner = this;
					SpawnParams.bNoFail = true;
					SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

					ActiveCollectibleActor = World->SpawnActor<ACollectibleActor>(CollectibleClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
		
					if (ensureAlways(IsValid(ActiveCollectibleActor)))
					{
						LastSpawnLocation = SpawnLocation;
						ActiveCollectibleActor->OnCollectedActor.AddUniqueDynamic(this, &ThisClass::HandleCollectibleCollected);
					}
				}
			}
		}
	}
}
