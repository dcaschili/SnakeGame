#include "SnakeGameGameModeBase.h"

#include "Game/MapManager.h"
#include "SnakeLog.h"

void ASnakeGameGameModeBase::StartPlay()
{
	// BeginPlay called on all actors
	Super::StartPlay();
}

void ASnakeGameGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("PostLogin"));
}

void ASnakeGameGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

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
