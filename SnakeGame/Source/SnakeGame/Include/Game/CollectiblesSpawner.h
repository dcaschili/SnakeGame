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
	void HandleOnMatchStarted();
	UFUNCTION()
	void HandleCollectibleCollected(const FVector& InCollectibleLocation);

	void InitializeCollectiblePool();
	void SpawnCollectible();
	
	/*
		Since Snake game has a single collectible at a time  
		It would have been possible to use a single actor. 
		With that implementation there were some problems with the overlap 
		interaction in the process of Disabling, Moving and Enabling 
		the actor: the overlap event was generated again after the last step.
		To avoid to introduce a delay between the moving and the enabling,
		i decided to use the following approach:
		I initalize a pool of two actors but only one of them is enabled
		and visibile at any given time. Once the player collects the active 
		collectible I simply enable the other.
	*/
	UPROPERTY()
	TArray<ACollectibleActor*>	CollectibleActorsPoll{};
	
	TOptional<int32>			ActiveCollectible{};
	TOptional<FVector>			LastSpawnLocation{};
	FVector						OutOfMapLocation{ 0.0f, 0.0f, -2000.0f };
	const int32					CollectiblePoolSize = 2;
};