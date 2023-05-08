#pragma once

#include "GameFramework/GameModeBase.h"
#include "SnakeGameGameModeBase.generated.h"

class AMapManager;
class ACollectiblesSpawner;

UCLASS()
class SNAKEGAME_API ASnakeGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	// After the super, the begin play has been called on all actors.
	virtual void StartPlay() override;
	// TODO: If all players are ready, start game 
	virtual void PostLogin(APlayerController* NewPlayer) override;
	// TODO: Create helper classes
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	AMapManager*			GetMapManager() { return SnakeMapManager; }
	ACollectiblesSpawner*	GetCollectiblesSpawner() { return SnakeCollectiblesSpawner; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame")
	TSubclassOf<AMapManager> SnakeMapManagerClass;
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame")
	TSubclassOf<ACollectiblesSpawner> SnakeCollectibleSpawnerClass;

private:
	void SpawnMapManager();
	void SpawnCollectiblesSpawner();


	UPROPERTY()
	TObjectPtr<AMapManager> SnakeMapManager = nullptr;
	UPROPERTY()
	TObjectPtr<ACollectiblesSpawner> SnakeCollectiblesSpawner = nullptr;
};
