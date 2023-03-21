#pragma once

#include "GameFramework/GameModeBase.h"
#include "SnakeGameGameModeBase.generated.h"

class AMapManager;

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

protected:
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame")
	TSubclassOf<AMapManager> SnakeMapManagerClass;

private:
	UPROPERTY()
	TObjectPtr<AMapManager> SnakeMapManager = nullptr;
};
