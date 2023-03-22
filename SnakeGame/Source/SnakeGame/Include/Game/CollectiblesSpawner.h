#pragma once

#include "GameFramework/Actor.h"

#include "CollectiblesSpawner.generated.h"

class ACollectibleActor;

UCLASS()
class SNAKEGAME_API ACollectiblesSpawner : public AActor
{
    GENERATED_BODY()
public:
	ACollectiblesSpawner();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Spawning")
	TSubclassOf<ACollectibleActor> CollectibleClass{};
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Spawning", meta=(MinUI = 0.0f, ClampMinUI = 0.0f))
	float SpawningStartingHeight = 40.0f;
	


private:
	UFUNCTION()
	void HandleCollectibleDestroyed();

	void SpawnCollectible();

	UPROPERTY()
	TObjectPtr<ACollectibleActor> ActiveCollectibleActor{};
};