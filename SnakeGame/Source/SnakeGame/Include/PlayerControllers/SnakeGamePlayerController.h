
#pragma once

#include "GameFramework/PlayerController.h"

#include "SnakeGamePlayerController.generated.h"

class UGDTUIUWBasePageLayout;
class UInputMappingContext;
class UInputAction;
struct FInputActionInstance;

UCLASS(Config=Game)
class SNAKEGAME_API ASnakeGamePlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	UPROPERTY()
	TObjectPtr<UGDTUIUWBasePageLayout> BaseLayoutPage{};

private:
	UFUNCTION()
	void HandleCloseGameAction(const FInputActionInstance& InputInstance);

	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|UI", meta = (EditCondition = "bOverrideDefaultLayoutPageClass"))
	TSubclassOf<UGDTUIUWBasePageLayout> LayoutPageClassOverride{};
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|UI")
	bool bOverrideDefaultLayoutPageClass = false;
	UPROPERTY(Config)
	TSubclassOf<UGDTUIUWBasePageLayout> DefaultLayoutPageClass{};
	
	// INPUT BINDING
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Inputs")
	TObjectPtr<UInputAction> CloseGameAction{};
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Inputs")
	TObjectPtr<UInputMappingContext> BaseControllerMappingContext{};


	void SetupBaseLayout();
};