#pragma once

#include "GameFramework/Actor.h"
#include "Game/SnakeBodyPartType.h"
#include "Containers/Queue.h"
#include "Game/ChangeDirectionAction.h"

#include "SnakeBodyPart.generated.h"

class ASnakePawn;
class USnakeBodyPartMoveComponent;

UCLASS()
class SNAKEGAME_API ASnakeBodyPart : public AActor
{
    GENERATED_BODY()
public:
	ASnakeBodyPart();

	virtual void Tick(float DeltaSeconds) override;

	void SetSnakePawn(ASnakePawn* InPawnPtr);
	void SetBodyPartType(ESnakeBodyPartType NewBodyPartType);

	ESnakeBodyPartType GetBodyPartType() const { return BodyPartType; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void HandleChangeDirectionDelegate(const FChangeDirectionAction& InNewChangeDirection);

	void BindDelegates();
	void UnbindDelegates();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SnakeGame|Body", meta=(AllowPrivateAccess=true))
	ESnakeBodyPartType BodyPartType {};

	UPROPERTY(EditDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComp{};
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<USnakeBodyPartMoveComponent> SnakeMovementComponent{};


	TWeakObjectPtr<ASnakePawn>		SnakePawnPtr{};
	TQueue<FChangeDirectionAction>	ChangeDirectionQueue{};
	float							HalfTileSize{};
};