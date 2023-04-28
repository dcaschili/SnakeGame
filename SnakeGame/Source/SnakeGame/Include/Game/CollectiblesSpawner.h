#pragma once

#include "GameFramework/Actor.h"

#include "CollectiblesSpawner.generated.h"

class ACollectibleActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollectibleSpawnerCollectedEvent, const FVector&, InCollectibleLocation);

UCLASS()
class SNAKEGAME_API ACollectiblesSpawner : public AActor
{
    GENERATED_BODY()
public:
	
	ACollectiblesSpawner();

	FCollectibleSpawnerCollectedEvent OnCollectibleCollected{};

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Spawning")
	TSubclassOf<ACollectibleActor> CollectibleClass{};
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Spawning", meta=(MinUI = 0.0f, ClampMinUI = 0.0f))
	float SpawningStartingHeight = 50.0f;


private:
	UFUNCTION()
	void HandleCollectibleCollected(const FVector& InCollectibleLocation);

	void SpawnCollectible();
	
	UPROPERTY()
	TObjectPtr<ACollectibleActor>		ActiveCollectibleActor{};

	TOptional<FVector>					LastSpawnLocation{};
};