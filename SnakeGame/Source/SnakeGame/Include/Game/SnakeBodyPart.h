#pragma once

#include "GameFramework/Actor.h"
#include "Game/SnakeBodyPartType.h"
#include "Containers/Queue.h"
#include "Game/ChangeDirectionAction.h"
#include "Game/SnakeBodyPartTypeInterface.h"

#include "SnakeBodyPart.generated.h"

class ASnakePawn;
class USnakeBodyPartMoveComponent;
class UMapOccupancyComponent;

UCLASS()
class SNAKEGAME_API ASnakeBodyPart : public AActor, public ISnakeBodyPartTypeInterface
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
	void							AddChangeDirAction(const FChangeDirectionAction& InChangeDirAction);
	TArray<FChangeDirectionAction>	GetChangeDirectionQueue() const { return ChangeDirectionQueue; }
	void							SetChangeDirQueue(const TArray<FChangeDirectionAction>& InChangeDirQueue) { ChangeDirectionQueue = InChangeDirQueue; }
	void							SetChangeDirQueue(TArray<FChangeDirectionAction>&& InChangeDirQueue) { ChangeDirectionQueue = MoveTemp(InChangeDirQueue); }
	
	
	// ISnakeBodyPartTypeInterface
	virtual void				SetSnakeBodyPartType(ESnakeBodyPartType InBodyPartType) override { BodyPartType = InBodyPartType; }
	virtual ESnakeBodyPartType	GetSnakeBodyPartType() const override { return BodyPartType; };

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

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
};