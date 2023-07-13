#pragma once

#include "Game/Snake/SnakePawn.h"

#include "SnakeMatchPawn.generated.h"

class UMapOccupancyComponent;
class UEndGameOverlapDetectionComponent;

UCLASS()
class SNAKEGAME_API ASnakeMatchPawn : public ASnakePawn
{
    GENERATED_BODY()
public:
	ASnakeMatchPawn();

	FORCEINLINE UEndGameOverlapDetectionComponent* GetEndGameOverlapDetectionComponent() const { return EndGameOverlapComponent; }

protected:
	virtual void BindEvents() override;
	virtual void UnbindEvents() override;

private:
	UFUNCTION()
	void HandleCollectibleCollected(const FVector& InCollectibleLocation);

    UPROPERTY(EditDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<UEndGameOverlapDetectionComponent> EndGameOverlapComponent{};
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<UMapOccupancyComponent> MapOccupancyComponent{};
};