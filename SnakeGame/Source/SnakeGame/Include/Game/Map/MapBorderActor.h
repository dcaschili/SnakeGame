#pragma once

#include "GameFramework/Actor.h"

#include "MapBorderActor.generated.h"

class UStaticMeshComponent;
class UEndGameCollisionDetectionComponent;

UCLASS()
class SNAKEGAME_API AMapBorderActor : public AActor
{
    GENERATED_BODY()
public:
	AMapBorderActor();	

private:
	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent{};
	
};