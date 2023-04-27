#pragma once

#include "GameFramework/Actor.h"

#include "SnakeBodyPartSpawner.generated.h"

class ASnakeBodyPart;
class UBoxComponent;
class ASnakePawn;
struct FChangeDirectionAction;

UCLASS()
class SNAKEGAME_API ASnakeBodyPartSpawner : public AActor
{
    GENERATED_BODY()
public:
	ASnakeBodyPartSpawner();

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

private:

	void SpawnBodyPart(ASnakePawn* InSnakePawn, const FVector& InMoveDirection, const TArray<FChangeDirectionAction>& InChangeDirectionQueue = {});

	UPROPERTY(EditDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<UBoxComponent> BoxCollisionComponent{};
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Spawn")
	TSubclassOf<ASnakeBodyPart> SnakeBodyPartClass{};

	bool bSpawnCompleted = false;
};