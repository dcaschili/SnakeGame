#include "Game/Map/MapBorderActor.h"

#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/HitResult.h"
#include "SnakeLog.h"


AMapBorderActor::AMapBorderActor()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Component"));
	RootComponent = StaticMeshComponent;

	StaticMeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);	

	StaticMeshComponent->CastShadow = false;
}

void AMapBorderActor::BeginPlay()
{
	Super::BeginPlay();

	check(StaticMeshComponent);
	StaticMeshComponent->OnComponentHit.AddUniqueDynamic(this, &ThisClass::HandleStaticMeshComponentHit);
}

void AMapBorderActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	check(StaticMeshComponent);
	StaticMeshComponent->OnComponentHit.RemoveDynamic(this, &ThisClass::HandleStaticMeshComponentHit);

	Super::EndPlay(EndPlayReason);
}

void AMapBorderActor::HandleStaticMeshComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor)
	{
		const FString Msg = FString::Printf(TEXT("Collided with: %s"), *OtherActor->GetName());
		UE_LOG(SnakeLogCategoryMap, Verbose, TEXT("%s"), *Msg);
	}
}

