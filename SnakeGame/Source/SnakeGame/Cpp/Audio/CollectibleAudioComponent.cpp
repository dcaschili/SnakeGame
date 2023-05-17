#include "Audio/CollectibleAudioComponent.h"

#include "Game/CollectibleActor.h"
#include "SnakeLog.h"

UCollectibleAudioComponent::UCollectibleAudioComponent()
	: Super()
{
	bAutoActivate = false;
}

void UCollectibleAudioComponent::BeginPlay()
{
	Super::BeginPlay();

	ACollectibleActor* const CollectibleOwner = Cast<ACollectibleActor>(GetOwner());
	if (CollectibleOwner)
	{
		CollectibleOwner->OnCollectedActor.AddUniqueDynamic(this, &ThisClass::HandleCollectibleCollected);
	}
	else
	{
		GDTUI_PRINT_TO_SCREEN_WARN(TEXT("Wrong owner class for UCollectibleAudioComponent!"));
		GDTUI_LOG(SnakeLogCategoryAudio, Warning, TEXT("Using UCollectibleAudioComponent on a non collectible class! This won't work!"));
		ensure(false);
	}
}

void UCollectibleAudioComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ACollectibleActor* const CollectibleOwner = Cast<ACollectibleActor>(GetOwner()))
	{
		CollectibleOwner->OnCollectedActor.RemoveDynamic(this, &ThisClass::HandleCollectibleCollected);
	}

	Super::EndPlay(EndPlayReason);
}

void UCollectibleAudioComponent::HandleCollectibleCollected(const FVector& InCollectibleLocation)
{
	Play();
}
