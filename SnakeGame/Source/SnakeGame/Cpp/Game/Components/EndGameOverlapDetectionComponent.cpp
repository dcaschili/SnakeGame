#include "Game/Components/EndGameOverlapDetectionComponent.h"

#include "GameFramework/Actor.h"
#include "Game/Interfaces/TriggerEndGameInterface.h"
#include "SnakeLog.h"
#include "Kismet/GameplayStatics.h"
#include "SnakeMatchGameModeBase.h"

#if !UE_BUILD_SHIPPING
#include "Engine.h"
#endif // !UE_BUILD_SHIPPING

UEndGameOverlapDetectionComponent::UEndGameOverlapDetectionComponent()
	: Super()
{
	SetIsReplicatedByDefault(false);
}

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
			if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("EndGame overlap detected!"));
#endif // !UE_BUILD_SHIPPING
			GDTUI_SHORT_LOG(SnakeLogCategoryGame, Log, TEXT("EndGame overlap detected!"));

			// Trigger endgame on the game mode
			if (ASnakeMatchGameModeBase* const SnakeGameMode = Cast<ASnakeMatchGameModeBase>(UGameplayStatics::GetGameMode(this)))
			{
				// Works only on the server.
				SnakeGameMode->EndMatch();
			}
		}
		else
		{
			GDTUI_SHORT_LOG(SnakeLogCategoryGame, Verbose, TEXT("Overlap detected! Not valid for end game!"));
		}
	}
}
