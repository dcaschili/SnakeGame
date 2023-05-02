#pragma once

#include "PlayerControllers/SnakeGamePlayerController.h"

#include "SnakeMenuPlayerController.generated.h"

class UGDTUIUWSimpleButtonListPage;
class UGDTUIUWBasePageLayout;

UCLASS()
class SNAKEGAME_API ASnakeMenuPlayerController : public ASnakeGamePlayerController
{
    GENERATED_BODY()

public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void BeginPlay() override;


	virtual void BindEvents();
	virtual void UnbindEvents();

	UPROPERTY(EditDefaultsOnly, Category="SnakeGame|UI")
	TSubclassOf<UGDTUIUWSimpleButtonListPage> MenuPageClass{};

	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|UI")
	TSubclassOf<UGDTUIUWBasePageLayout> BasePageLayoutClass{};

private:
	UFUNCTION()
	void HandleButtonClicked(const FName& InButtonId);

	UPROPERTY()
	TObjectPtr<UGDTUIUWSimpleButtonListPage> ButtonListPage{};
	UPROPERTY()
	TObjectPtr<UGDTUIUWBasePageLayout> BasePageLayoutPage{};
};