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
class USnakeBodyPartMoveComponent;
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
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
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
	
	//FORCEINLINE UStaticMeshComponent*	GetStaticMeshComponent() const { return StaticMeshComp; }
	FORCEINLINE USplineMeshComponent* GetSplineMeshComponent() const { return SplineMeshComp; }
	
	FORCEINLINE void SetTriggerEndGameOverlapEvent(bool bEnabled) { bTriggerEndGameOverlapEvent = bEnabled; }
	FORCEINLINE bool GetTriggerEndGameOverlapEvent() const { return bTriggerEndGameOverlapEvent; }

	void SetSnakeBodyPartIndex(int32 InBodyPartIndex);


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
	
	void UpdateSplineMeshComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SnakeGame|Body", meta=(AllowPrivateAccess=true))
	ESnakeBodyPartType BodyPartType {};

	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Components")
	TObjectPtr<USplineMeshComponent> SplineMeshComp{};
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Components")
	TObjectPtr<UBoxComponent> SnakeBodyPartCollider{};
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Components")
	TObjectPtr<USnakeBodyPartMoveComponent> SnakeMovementComponent{};
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Components")
	TObjectPtr<UMapOccupancyComponent> MapOccupancyComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Body")
	TEnumAsByte<ESplineMeshAxis::Type> SplineMeshAxis = ESplineMeshAxis::Type::Z;
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Body")
	TObjectPtr<UStaticMesh> SplineStaticMesh{};


	TObjectPtr<ASnakePawn>			SnakePawnPtr{};
	TArray<FChangeDirectionAction>	ChangeDirectionQueue{};
	float							HalfTileSize{};

	bool bTriggerEndGameOverlapEvent = true;

	TOptional<int32> BodyPartIndex{};
};