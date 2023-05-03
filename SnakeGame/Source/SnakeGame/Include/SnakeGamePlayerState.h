#pragma once

#include "GameFramework/PlayerState.h"

#include "SnakeGamePlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerStateScoreUpdatedDelegate);

UCLASS()
class SNAKEGAME_API ASnakeGamePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void OnRep_Score() override;

	FPlayerStateScoreUpdatedDelegate OnPlayerStateScoreUpdated{};

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void HandleCollectibleCollected(const FVector& InCollectibleLocation);
};