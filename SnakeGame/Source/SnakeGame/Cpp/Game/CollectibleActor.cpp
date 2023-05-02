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

void ACollectibleActor::EnableCollectible()
{
	SetActorEnableCollision(true);
	SetActorHiddenInGame(false);

	if (ensure(MapOccupancyComponent))
	{
		MapOccupancyComponent->ForceRefreshOccupancy();
	}
}

void ACollectibleActor::DisableCollectible()
{
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);

	if (ensure(MapOccupancyComponent))
	{
		MapOccupancyComponent->ForceFreeOccupancy();
	}
}

void ACollectibleActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (HasAuthority())
	{
		OnCollectedActor.Broadcast(GetActorLocation());
		//Destroy();
	}
}
