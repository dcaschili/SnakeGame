#pragma once

#include "PlayerControllers/SnakeGamePlayerController.h"

#include "SnakeMatchPlayerController.generated.h"

class UGDTUIUWBasePage;

UCLASS()
class SNAKEGAME_API ASnakeMatchPlayerController : public ASnakeGamePlayerController
{
    GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|UI")
	TSubclassOf<UGDTUIUWBasePage> HUDMatchPageClass{};

private:
	UPROPERTY()
	TObjectPtr<UGDTUIUWBasePage> HUDPage{};
};