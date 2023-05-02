#include "Game/Map/MapBorderActor.h"

#include "Components/StaticMeshComponent.h"

AMapBorderActor::AMapBorderActor()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Component"));
	RootComponent = StaticMeshComponent;

	StaticMeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);	

	StaticMeshComponent->CastShadow = false;
}




