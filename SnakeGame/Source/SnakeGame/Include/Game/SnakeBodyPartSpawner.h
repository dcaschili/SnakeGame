#pragma once

#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"

#include "SnakeBodyPartSpawner.generated.h"

UCLASS()
class SNAKEGAME_API ASnakeBodyPartSpawner : public AActor
{
    GENERATED_BODY()
public:
	ASnakeBodyPartSpawner();

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<UBoxComponent> BoxCollisionComponent{};
};