#pragma once

#include "GameFramework/Actor.h"

#include "MapBorderActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class SNAKEGAME_API AMapBorderActor : public AActor
{
    GENERATED_BODY()
public:
	AMapBorderActor();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void HandleStaticMeshComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent{};
};