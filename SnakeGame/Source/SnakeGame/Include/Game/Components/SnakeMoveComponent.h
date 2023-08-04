#pragma once

#include "Components/ActorComponent.h"
#include "Game/ChangeDirectionAction.h"

#include "SnakeMoveComponent.generated.h"

class AController;

UCLASS()
class SNAKEGAME_API USnakeMoveComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    USnakeMoveComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetMoveDir(const FVector& InNewDir);
	void SetUpdateControlRotation(bool bEnabled) { bUpdateControlRotation = bEnabled; }

	void							AddChangeDirAction(const FChangeDirectionAction& InChangeDirection);
	void							SetChangeDirActionQueue(const TArray<FChangeDirectionAction>& InChangeDirQueue);
	void							SetChangeDirActionQueue(TArray<FChangeDirectionAction>&& InChangeDirQueue);
	bool							IsChangeDirActionPending() const { return !ChangeDirectionQueue.IsEmpty(); }
	TArray<FChangeDirectionAction>	GetChangeDirActionQueue() const { return ChangeDirectionQueue; }


	FVector GetMoveDirection() const { return MoveDirection; }
	
private:
	AController* GetOwningController() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SnakeGame|Movement", meta = (AllowPrivateAccess = true))
	bool bUpdateControlRotation = false;
	
	FVector							MoveDirection = FVector::RightVector;
	TArray<FChangeDirectionAction>	ChangeDirectionQueue{};
}; 