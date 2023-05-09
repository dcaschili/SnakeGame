#pragma once

#include "PlayerControllers/SnakeGamePlayerController.h"

#include "SnakeMatchPlayerController.generated.h"

class UGDTUIUWBasePage;
class UGameOverPage;
class UInputAction;
class UInputMappingContext;
class ASnakePawn;

UCLASS()
class SNAKEGAME_API ASnakeMatchPlayerController : public ASnakeGamePlayerController
{
    GENERATED_BODY()

public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	virtual void InnerHandleEndMatch();

	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|UI")
	TSubclassOf<UGDTUIUWBasePage> HUDMatchPageClass{};
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|UI")
	TSubclassOf<UGameOverPage> GameOverPageClass{};

	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Inputs")
	TObjectPtr<UInputAction> StartMatchIA{};
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Inputs")
	TObjectPtr<UInputMappingContext> InputMappingContext{};
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Game")
	TSubclassOf<ASnakePawn> SnakePawnClass{};

private:
	UFUNCTION()
	void HandleStartMatchAction(const FInputActionInstance& InputActionInstance);
	UFUNCTION()
	void HandleEndGamePageButtonClicked(const FName& InButtonId);
	UFUNCTION()
	void HandleEndMatchDelegate();
	UFUNCTION()
	void HandleStartMatchDelegate();
	UFUNCTION()
	void HandleScoreChanged();

	/*
		RPC
	*/
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartMatch();
	/* Neede to forward the end game to all clients to change page */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EndMatch();

	
	void UpdatePageScore();


	UPROPERTY(EditDefaultsOnly, Category="SnakeGame|UI")
	FName GameOverPageContinueButtonId{};
};