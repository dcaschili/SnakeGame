#include "Game/CollectibleActor.h"

#include "Components/StaticMeshComponent.h"
#include "Game/Map/MapOccupancyComponent.h"
#include "Game/Snake/SnakePawn.h"
#include "Audio/CollectibleAudioComponent.h"

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
	CollectibleAudioComponent = CreateDefaultSubobject<UCollectibleAudioComponent>(TEXT("CollectibleAudioComponent"));
}

void ACollectibleActor::EnableCollectible()
{
	if (ensure(StaticMeshComp))
	{
		StaticMeshComp->SetGenerateOverlapEvents(true);
	}

	SetActorHiddenInGame(false);

	if (ensure(MapOccupancyComponent))
	{
		MapOccupancyComponent->ForceRefreshOccupancy();
	}
}

void ACollectibleActor::DisableCollectible()
{
	if (ensure(StaticMeshComp))
	{
		StaticMeshComp->SetGenerateOverlapEvents(false);
	}

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
		if (ASnakePawn* const SnakePawn = Cast<ASnakePawn>(OtherActor))
		{
			DisableCollectible();
			OnCollectedActor.Broadcast(GetActorLocation());
		}
	}
}
