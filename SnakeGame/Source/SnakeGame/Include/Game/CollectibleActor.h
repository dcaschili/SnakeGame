#pragma once

#include "GameFramework/Actor.h"

#include "CollectibleActor.generated.h"

class UMapOccupancyComponent;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollectedActorEvent, const FVector&, InCollectibleLocation);

UCLASS()
class SNAKEGAME_API ACollectibleActor : public AActor
{
    GENERATED_BODY()
public:
	ACollectibleActor();

	void DisableCollectible();
	void EnableCollectible();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	FCollectedActorEvent OnCollectedActor{};

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComp{};
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<UMapOccupancyComponent> MapOccupancyComponent{};
};