#include "Audio/SnakeChangeDirectionAudioComponent.h"

#include "Game/SnakePawn.h"
#include "SnakeLog.h"

USnakeChangeDirectionAudioComponent::USnakeChangeDirectionAudioComponent()
	: Super()
{
	bAutoActivate = false;
}

void USnakeChangeDirectionAudioComponent::BeginPlay()
{
	Super::BeginPlay();

	ASnakePawn* const SnakePawn = Cast<ASnakePawn>(GetOwner());
	if (SnakePawn)
	{
		SnakePawn->OnChangeDirection.AddUniqueDynamic(this, &ThisClass::HandleChangeDirection);
	}
	else
	{
		GDTUI_PRINT_TO_SCREEN_WARN(TEXT("Wrong owner class for USnakeChangeDirectionAudioComponent!"));
		GDTUI_LOG(SnakeLogCategoryAudio, Warning, TEXT("Wrong owner class for USnakeChangeDirectionAudioComponent!"));
		ensure(false);
	}
}

void USnakeChangeDirectionAudioComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ASnakePawn* const SnakePawn = Cast<ASnakePawn>(GetOwner());
	if (SnakePawn)
	{
		SnakePawn->OnChangeDirection.RemoveDynamic(this, &ThisClass::HandleChangeDirection);
	}

	Super::EndPlay(EndPlayReason);
}

void USnakeChangeDirectionAudioComponent::HandleChangeDirection(const FChangeDirectionAction& NewDirectionAction)
{
	Play();
}

