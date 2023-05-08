
#pragma once

#include "GameFramework/PlayerController.h"

#include "SnakeGamePlayerController.generated.h"

class UGDTUIUWBasePageLayout;

UCLASS()
class SNAKEGAME_API ASnakeGamePlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|UI")
	TSubclassOf<UGDTUIUWBasePageLayout> BaseLayoutPageClass{};
	
	UPROPERTY()
	TObjectPtr<UGDTUIUWBasePageLayout> BaseLayoutPage{};

private:
	void SetupBaseLayout();
};