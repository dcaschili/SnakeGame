
#pragma once

#include "GameFramework/PlayerController.h"

#include "SnakeGamePlayerController.generated.h"

class UGDTUIUWBasePageLayout;

UCLASS(Config=Game)
class SNAKEGAME_API ASnakeGamePlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	
	UPROPERTY()
	TObjectPtr<UGDTUIUWBasePageLayout> BaseLayoutPage{};

private:
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|UI", meta = (EditCondition = "bOverrideDefaultLayoutPageClass"))
	TSubclassOf<UGDTUIUWBasePageLayout> LayoutPageClassOverride{};
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|UI")
	bool bOverrideDefaultLayoutPageClass = false;
	UPROPERTY(Config)
	TSubclassOf<UGDTUIUWBasePageLayout> DefaultLayoutPageClass{};


	void SetupBaseLayout();
};