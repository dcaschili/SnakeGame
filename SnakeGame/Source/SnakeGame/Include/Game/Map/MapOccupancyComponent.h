#pragma once

#include "Components/ActorComponent.h"

#include "MapOccupancyComponent.generated.h"

UCLASS(ClassGroup = (SnakeGameComponent), EditInLineNew, meta = (BlueprintSpawnableComponent))
class SNAKEGAME_API UMapOccupancyComponent : public UActorComponent
{
    GENERATED_BODY()
public:
	UMapOccupancyComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private: 
	void HandleOccupancyTest();

	bool IsOccupancyUpdateNeeded() const;
	void UpdateOccupancy();

	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|OccupancyTest")
	bool bEnableContinuousTileOccupancyTest = false;

	TOptional<FIntVector2> PreviousMapTileLocation{};

	FTimerHandle OccupacyTestTimerHandle{};
};