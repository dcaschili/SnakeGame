#include "Game/CollectibleActor.h"

#include "Components/StaticMeshComponent.h"
#include "Game/Map/MapOccupancyComponent.h"

ACollectibleActor::ACollectibleActor()
	: Super()
{
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	if (StaticMeshComp)
	{
		RootComponent = StaticMeshComp;
		StaticMeshComp->SetGenerateOverlapEvents(true);
	}

	MapOccupancyComponent = CreateDefaultSubobject<UMapOccupancyComponent>(TEXT("MapOccupancyComponent"));
}

void ACollectibleActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (HasAuthority())
	{
		OnCollectedActor.Broadcast(GetActorLocation());
		Destroy();
	}
}
