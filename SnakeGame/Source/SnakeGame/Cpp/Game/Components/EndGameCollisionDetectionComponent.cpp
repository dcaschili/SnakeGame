#include "Game/Components/EndGameCollisionDetectionComponent.h"

#include "Components/StaticMeshComponent.h"
#include "SnakeLog.h"
#include "Game/Map/MapBorderActor.h"

void UEndGameCollisionDetectionComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* const Owner = GetOwner();
	OwnerStaticMeshComponent = Owner ? Cast<UStaticMeshComponent>(Owner->GetComponentByClass(UStaticMeshComponent::StaticClass())) : nullptr;

	if (ensure(OwnerStaticMeshComponent.IsValid()))
	{
		OwnerStaticMeshComponent->OnComponentHit.AddUniqueDynamic(this, &ThisClass::HandleStaticMeshComponentHit);
	}
}

void UEndGameCollisionDetectionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (OwnerStaticMeshComponent.IsValid())
	{
		OwnerStaticMeshComponent->OnComponentHit.RemoveDynamic(this, &ThisClass::HandleStaticMeshComponentHit);
	}

	Super::EndPlay(EndPlayReason);
}

void UEndGameCollisionDetectionComponent::HandleStaticMeshComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor)
	{
		const FString Msg = FString::Printf(TEXT("Collided with: %s"), *OtherActor->GetName());
		UE_LOG(SnakeLogCategoryGame, Verbose, TEXT("%s"), *Msg);

		if (AMapBorderActor* MapBorder = Cast<AMapBorderActor>(OtherActor))
		{
			//UE_LOG(SnakeLogCategoryMap, Verbose, TEXT("Collision with wall, EndGame!"));
		}
		// TODO: Add collision with body parts.
	}
}
