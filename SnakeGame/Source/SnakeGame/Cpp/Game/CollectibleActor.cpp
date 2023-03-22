#include "Game/CollectibleActor.h"

#include "Components/StaticMeshComponent.h"

ACollectibleActor::ACollectibleActor()
	: Super()
{
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	if (StaticMeshComp)
	{
		RootComponent = StaticMeshComp;
		StaticMeshComp->SetGenerateOverlapEvents(true);
	}
}

void ACollectibleActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (HasAuthority())
	{
		OnCollectedActor.Broadcast();
		Destroy();
	}
}
