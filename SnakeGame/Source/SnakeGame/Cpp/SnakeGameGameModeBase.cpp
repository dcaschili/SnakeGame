#include "SnakeGameGameModeBase.h"

#include "Game/MapManager.h"
#include "SnakeLog.h"
#include "Game/CollectiblesSpawner.h"
#include "Engine/World.h"

void ASnakeGameGameModeBase::StartPlay()
{
	// BeginPlay called on all actors
	Super::StartPlay();
}

void ASnakeGameGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void ASnakeGameGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	SpawnMapManager();
	SpawnCollectiblesSpawner();
}

void ASnakeGameGameModeBase::SpawnMapManager()
{
	if (GetWorld())
	{
		TSubclassOf<AMapManager> TmpClass = SnakeMapManagerClass;
		if (!IsValid(TmpClass))
		{
			TmpClass = AMapManager::StaticClass();
		}
		UE_LOG(SnakeLogCategoryMap, Log, TEXT("Spawned map manager helper!"));
		SnakeMapManager = GetWorld()->SpawnActor<AMapManager>(TmpClass);
	}
}

void ASnakeGameGameModeBase::SpawnCollectiblesSpawner()
{
	if (GetWorld())
	{
		TSubclassOf<ACollectiblesSpawner> TmpClass = SnakeCollectibleSpawnerClass;
		if (!IsValid(TmpClass))
		{
			TmpClass = ACollectiblesSpawner::StaticClass();
		}
		UE_LOG(SnakeLogCategorySpawner, Log, TEXT("Spawned collectibles spawner helper!"));
		SnakeCollectiblesSpawner = GetWorld()->SpawnActor<ACollectiblesSpawner>(TmpClass);
	}
}
