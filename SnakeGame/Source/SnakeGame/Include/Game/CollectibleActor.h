#pragma once

#include "GameFramework/Actor.h"

#include "CollectibleActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class SNAKEGAME_API ACollectibleActor : public AActor
{
    GENERATED_BODY()
public:
	ACollectibleActor();

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> StaticMeshComp{};
};