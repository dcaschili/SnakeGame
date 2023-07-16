#pragma once

#include "GameFramework/Actor.h"
#include "Game/Snake/SnakeBodyPartType.h"
#include "Containers/Queue.h"
#include "Game/ChangeDirectionAction.h"
#include "Game/Interfaces/SnakeBodyPartTypeInterface.h"
#include "Game/Interfaces/TriggerEndGameInterface.h"

#include "SnakeBodyPart.generated.h"

class ASnakePawn;
class USnakeBodyPartMoveComponent;
class UMapOccupancyComponent;

UCLASS()
class SNAKEGAME_API ASnakeBodyPart : public AActor, public ISnakeBodyPartTypeInterface, public ITriggerEndGameInterface
{
    GENERATED_BODY()
public:
	ASnakeBodyPart();

	virtual void Tick(float DeltaSeconds) override;

	void		SetMoveDir(const FVector& InMoveDirection);
	FVector		GetMoveDirection() const;
	void		SetSnakePawn(ASnakePawn* InPawnPtr);
	ASnakePawn* GetSnakePawn() const;
	
	/** Enqueue a change direction action */
	void								AddChangeDirAction(const FChangeDirectionAction& InChangeDirAction);
	TArray<FChangeDirectionAction>		GetChangeDirectionQueue() const { return ChangeDirectionQueue; }
	void								SetChangeDirQueue(const TArray<FChangeDirectionAction>& InChangeDirQueue) { ChangeDirectionQueue = InChangeDirQueue; }
	void								SetChangeDirQueue(TArray<FChangeDirectionAction>&& InChangeDirQueue) { ChangeDirectionQueue = MoveTemp(InChangeDirQueue); }
	
	FORCEINLINE UStaticMeshComponent*	GetStaticMeshComponent() const { return StaticMeshComp; }
	
	FORCEINLINE void SetTriggerEndGameOverlapEvent(bool bEnabled) { bTriggerEndGameOverlapEvent = bEnabled; }
	FORCEINLINE bool GetTriggerEndGameOverlapEvent() const { return bTriggerEndGameOverlapEvent; }

	/*
		INTERFACES
	*/

	// ISnakeBodyPartTypeInterface
	virtual void						SetSnakeBodyPartType(ESnakeBodyPartType InBodyPartType) override { BodyPartType = InBodyPartType; }
	virtual ESnakeBodyPartType			GetSnakeBodyPartType() const override { return BodyPartType; };

	// ITriggerEndGameInterface
	virtual bool ShouldTriggerEndGame() const override { return GetTriggerEndGameOverlapEvent(); }
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void HandleChangeDirectionAction(const FChangeDirectionAction& NewDirectionAction);

	void BindPawnDelegates();
	void UnbindPawnDelegates();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SnakeGame|Body", meta=(AllowPrivateAccess=true))
	ESnakeBodyPartType BodyPartType {};

	UPROPERTY(EditDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComp{};
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<USnakeBodyPartMoveComponent> SnakeMovementComponent{};
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<UMapOccupancyComponent> MapOccupancyComponent{};


	TWeakObjectPtr<ASnakePawn>		SnakePawnPtr{};
	TArray<FChangeDirectionAction>	ChangeDirectionQueue{};
	float							HalfTileSize{};

	bool bTriggerEndGameOverlapEvent = true;

	
};