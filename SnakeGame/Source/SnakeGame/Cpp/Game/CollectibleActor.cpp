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

	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, TEXT("Overlap!"));
	
	OnCollectedActor.Broadcast();

	Destroy();
}
