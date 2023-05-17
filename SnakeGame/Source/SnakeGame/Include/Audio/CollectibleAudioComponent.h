#pragma once

#include "Components/AudioComponent.h"

#include "CollectibleAudioComponent.generated.h"

UCLASS()
class SNAKEGAME_API UCollectibleAudioComponent : public UAudioComponent
{
    GENERATED_BODY()

public:
	UCollectibleAudioComponent();

	// UActorComponent
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void HandleCollectibleCollected(const FVector& InCollectibleLocation);
};