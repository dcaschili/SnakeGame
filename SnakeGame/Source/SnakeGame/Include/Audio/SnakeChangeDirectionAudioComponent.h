#pragma once

#include "Components/AudioComponent.h"

#include "SnakeChangeDirectionAudioComponent.generated.h"

UCLASS()
class SNAKEGAME_API USnakeChangeDirectionAudioComponent : public UAudioComponent
{
    GENERATED_BODY()

public:
	USnakeChangeDirectionAudioComponent();

	// UActorComponent
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void HandleChangeDirection(const FChangeDirectionAction& NewDirectionAction);
};