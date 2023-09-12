#include "Game/Map/MapOccupancyComponent.h"

#include "Game/Map/MapManager.h"
#include "Game/Map/MapFunctionLibrary.h"
#include "SnakeLog.h"
#include "TimerManager.h"
#include "Engine/World.h"

#if WITH_EDITOR
#include "Engine.h"
#endif // WITH_EDITOR

UMapOccupancyComponent::UMapOccupancyComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMapOccupancyComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickEnabled(bEnableContinuousTileOccupancyTest);

	HandleOccupancyTest();
}

void UMapOccupancyComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{	
	if (PreviousMapTileLocation.IsSet())
	{
		if (AMapManager* const MapManager = AMapManager::GetMapManager(this))
		{
			MapManager->ReduceTileOccupancy(PreviousMapTileLocation.GetValue());
		}
	}

	Super::EndPlay(EndPlayReason);
}

void UMapOccupancyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT("MapOccupancyComponent"));
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	HandleOccupancyTest();
}

void UMapOccupancyComponent::ForceFreeOccupancy()
{
	AMapManager* const MapManager = AMapManager::GetMapManager(this);

	if (MapManager && PreviousMapTileLocation.IsSet())
	{
		MapManager->ReduceTileOccupancy(PreviousMapTileLocation.GetValue());
		PreviousMapTileLocation.Reset();
	}
}

void UMapOccupancyComponent::ForceRefreshOccupancy()
{
	UpdateOccupancy();
}

void UMapOccupancyComponent::HandleOccupancyTest()
{
	if (IsOccupancyUpdateNeeded())
	{
		UpdateOccupancy();
	}
}

bool UMapOccupancyComponent::IsOccupancyUpdateNeeded() const
{
	const AActor* const Owner = GetOwner();
	check(Owner);

	FIntVector2 CurrentMapTileLocation{};
	const bool bMapLocationExists = UMapFunctionLibrary::GetMapTileFromWorldLocation(this, Owner->GetActorLocation(), CurrentMapTileLocation);
	
	if (!bMapLocationExists)
	{
		FString Msg = FString::Printf(TEXT("Location: %s is out of map boundaries"), *Owner->GetActorLocation().ToString());
		UE_LOG(SnakeLogCategoryMap, Error, TEXT("%s"), *Msg);
#if WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, Msg);
#endif // WITH_EDITOR
		ensure(false);
		return false;
	}

	/*
		If a previous tile map location exists and it is different than the current one, update the occupancy.
		Otherwise update it only if the current position has a corresponding map position.
	*/
	return PreviousMapTileLocation.IsSet() ? (CurrentMapTileLocation != PreviousMapTileLocation.GetValue()) : bMapLocationExists;
}

void UMapOccupancyComponent::UpdateOccupancy()
{
	AMapManager* const MapManager = AMapManager::GetMapManager(this);
	if (MapManager)
	{
		// Remove occupancy from previous tile
		ForceFreeOccupancy();

		// Set occupancy in current tile
		const AActor* const Owner = GetOwner();
		check(Owner);
		FIntVector2 NewMapLocation{};
		if (ensure(UMapFunctionLibrary::GetMapTileFromWorldLocation(this, Owner->GetActorLocation(), NewMapLocation)))
		{
			MapManager->IncrementTileOccupancy(NewMapLocation);
		}

		// store current tile location
		PreviousMapTileLocation = NewMapLocation;
	}
}



