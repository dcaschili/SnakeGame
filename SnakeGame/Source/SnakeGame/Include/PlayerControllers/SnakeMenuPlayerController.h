#pragma once

#include "PlayerControllers/SnakeGamePlayerController.h"

#include "SnakeMenuPlayerController.generated.h"

class UGDTUIUWSimpleButtonListPage;
class UGDTUIUWBasePageLayout;

UENUM(BlueprintType)
enum class EMenuAction : uint8
{
	kStart     UMETA(DisplayName = "Start"),
	kExit      UMETA(DisplayName = "Exit")
};

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

	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Actions")
	TMap<FName, EMenuAction> ButtonIdToMenuAction{};

private:
	UFUNCTION()
	void HandleButtonClicked(const FName& InButtonId);
	
};