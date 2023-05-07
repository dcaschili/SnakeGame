#include "Game/Components/EndGameOverlapDetectionComponent.h"

#include "GameFramework/Actor.h"
#include "Game/TriggerEndGameInterface.h"
#include "SnakeLog.h"

void UEndGameOverlapDetectionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* const Owner = GetOwner())
	{
		Owner->OnActorBeginOverlap.AddUniqueDynamic(this, &ThisClass::HandleBeginOverlap);
	}
}

void UEndGameOverlapDetectionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AActor* const Owner = GetOwner())
	{
		Owner->OnActorBeginOverlap.RemoveDynamic(this, &ThisClass::HandleBeginOverlap);
	}

	Super::EndPlay(EndPlayReason);
}

void UEndGameOverlapDetectionComponent::HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (ITriggerEndGameInterface* Interface = Cast<ITriggerEndGameInterface>(OtherActor))
	{
		if (Interface->ShouldTriggerEndGame())
		{
#if !UE_BUILD_SHIPPING
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("Game Over!"));
#endif // !UE_BUILD_SHIPPING
			UE_LOG(SnakeLogCategoryGame, Log, TEXT("Game Over!"));

			OnEndGameOverlap.Broadcast();
		}
	}
}
