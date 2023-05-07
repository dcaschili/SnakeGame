#pragma once

#include "Components/ActorComponent.h"

#include "EndGameOverlapDetectionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndGameOverlapDelegate);

UCLASS()
class SNAKEGAME_API UEndGameOverlapDetectionComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	FOnEndGameOverlapDelegate OnEndGameOverlap{};

private:
	UFUNCTION()
	void HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

};