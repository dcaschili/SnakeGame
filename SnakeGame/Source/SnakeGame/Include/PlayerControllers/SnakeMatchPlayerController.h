#pragma once

#include "PlayerControllers/SnakeGamePlayerController.h"

#include "SnakeMatchPlayerController.generated.h"

class UGDTUIUWBasePage;

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

private:
	UFUNCTION()
	void HandleScoreChanged();

	void UpdatePageScore();

	UPROPERTY()
	TObjectPtr<UGDTUIUWBasePage> HUDPage{};
};