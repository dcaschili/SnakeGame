#pragma once

#include "GameFramework/Actor.h"
#include "Game/TriggerEndGameInterface.h"

#include "MapBorderActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class SNAKEGAME_API AMapBorderActor : public AActor, public ITriggerEndGameInterface
{
    GENERATED_BODY()
public:
	AMapBorderActor();	

private:
	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent{};
	
};