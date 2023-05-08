#pragma once

#include "PlayerControllers/SnakeGamePlayerController.h"

#include "SnakeMatchPlayerController.generated.h"

class UGDTUIUWBasePage;
class UGameOverPage;

UCLASS()
class SNAKEGAME_API ASnakeMatchPlayerController : public ASnakeGamePlayerController
{
    GENERATED_BODY()

public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|UI")
	TSubclassOf<UGDTUIUWBasePage> HUDMatchPageClass{};
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|UI")
	TSubclassOf<UGameOverPage> GameOverPageClass{};

private:
	UFUNCTION()
	void HandleEndGameDelegate();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EndGame();
	
	virtual void InnerHandleEndGame();

	UFUNCTION()
	void HandleScoreChanged();

	void UpdatePageScore();
};