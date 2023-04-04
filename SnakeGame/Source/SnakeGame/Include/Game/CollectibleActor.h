#pragma once

#include "GameFramework/Actor.h"

#include "CollectibleActor.generated.h"

class UMapOccupancyComponent;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCollectedActorEvent);

UCLASS()
class SNAKEGAME_API ACollectibleActor : public AActor
{
    GENERATED_BODY()
public:
	ACollectibleActor();

	FCollectedActorEvent OnCollectedActor{};

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComp{};
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<UMapOccupancyComponent> MapOccupancyComponent{};
};