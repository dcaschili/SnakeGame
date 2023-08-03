#pragma once

#include "GameFramework/Actor.h"
#include "Game/Snake/SnakeBodyPartType.h"
#include "Containers/Queue.h"
#include "Game/ChangeDirectionAction.h"
#include "Game/Interfaces/SnakeBodyPartTypeInterface.h"
#include "Game/Interfaces/TriggerEndGameInterface.h"
#include "Components/SplineMeshComponent.h"

#include "SnakeBodyPart.generated.h"

class ASnakePawn;
class USnakeMoveComponent;
class UMapOccupancyComponent;
class UBoxComponent;
class UStaticMesh;

UCLASS()
class SNAKEGAME_API ASnakeBodyPart : public AActor, public ISnakeBodyPartTypeInterface, public ITriggerEndGameInterface
{
    GENERATED_BODY()
public:
	ASnakeBodyPart();

	// AActor
	virtual void Tick(float DeltaSeconds) override;
	// !AActor

	void		SetMoveDir(const FVector& InMoveDirection);
	FVector		GetMoveDirection() const;
	void		SetSnakePawn(ASnakePawn* InPawnPtr);
	ASnakePawn* GetSnakePawn() const;
	
	/** Enqueue a change direction action */
	void								AddChangeDirAction(const FChangeDirectionAction& InChangeDirAction);
	TArray<FChangeDirectionAction>		GetChangeDirectionQueue() const { return ChangeDirectionQueue; }
	void								SetChangeDirQueue(const TArray<FChangeDirectionAction>& InChangeDirQueue) { ChangeDirectionQueue = InChangeDirQueue; }
	void								SetChangeDirQueue(TArray<FChangeDirectionAction>&& InChangeDirQueue) { ChangeDirectionQueue = MoveTemp(InChangeDirQueue); }
	
	FORCEINLINE void SetTriggerEndGameOverlapEvent(bool bEnabled) { bTriggerEndGameOverlapEvent = bEnabled; }
	FORCEINLINE bool GetTriggerEndGameOverlapEvent() const { return bTriggerEndGameOverlapEvent; }

	/*
		INTERFACES
	*/

	// ISnakeBodyPartTypeInterface
	virtual void						SetSnakeBodyPartType(ESnakeBodyPartType InBodyPartType) override;
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SnakeGame|Body", meta=(AllowPrivateAccess=true))
	ESnakeBodyPartType BodyPartType {};
	UPROPERTY(EditAnywhere, Category = "SnakeGame|Body")
	float SnakeBodyBoxColliderExtent = 40.0f;
	UPROPERTY(EditAnywhere, Category = "SnakeGame|Body|Debug")
	float SnakeBodyBoxColliderThickness = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Components")
	TObjectPtr<UBoxComponent> SnakeBodyPartCollider{};
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Components")
	TObjectPtr<UMapOccupancyComponent> MapOccupancyComponent{};



	TObjectPtr<ASnakePawn>			SnakePawnPtr{};
	TArray<FChangeDirectionAction>	ChangeDirectionQueue{};
	FVector							MoveDirection = FVector::RightVector;
	float							HalfTileSize{};

	bool bTriggerEndGameOverlapEvent = true;
};