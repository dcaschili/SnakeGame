#pragma once

#include "Components/ActorComponent.h"

#include "EndGameCollisionDetectionComponent.generated.h"

class UStaticMeshComponent;

UCLASS()
class SNAKEGAME_API UEndGameCollisionDetectionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void HandleStaticMeshComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	TWeakObjectPtr<UStaticMeshComponent> OwnerStaticMeshComponent{};
};