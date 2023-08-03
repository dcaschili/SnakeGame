#pragma once

#include "Components/ActorComponent.h"

#include "SnakeBodyPartMoveComponent.generated.h"

class AController;

UCLASS()
class SNAKEGAME_API USnakeBodyPartMoveComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    USnakeBodyPartMoveComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ChangeMoveDirection(const FVector& InNewDirection);
	void SetUpdateControlRotation(bool bEnabled) { bUpdateControlRotation = bEnabled; }

	FVector GetMoveDirection() const { return MoveDirection; }

private:
	AController* GetOwningController() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SnakeGame|Movement", meta = (AllowPrivateAccess = true))
	bool bUpdateControlRotation = false;
	
	bool						bDirectionChanged = false;
	FVector						MoveDirection = FVector::RightVector;
}; 