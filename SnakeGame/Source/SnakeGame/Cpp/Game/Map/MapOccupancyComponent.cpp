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
	if(ensure(PreviousMapTileLocation.IsSet()))
	if (AMapManager* const MapManager = AMapManager::GetMapManager(this))
	{
		MapManager->ReduceTileOccupancy(PreviousMapTileLocation.GetValue());
	}

	Super::EndPlay(EndPlayReason);
}

void UMapOccupancyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HandleOccupancyTest();
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

	return PreviousMapTileLocation.IsSet() ? (CurrentMapTileLocation != PreviousMapTileLocation.GetValue()) : bMapLocationExists;
}

void UMapOccupancyComponent::UpdateOccupancy()
{
	AMapManager* const MapManager = AMapManager::GetMapManager(this);

	// Remove occupancy from previous tile
	if (PreviousMapTileLocation.IsSet())
	{
		MapManager->ReduceTileOccupancy(PreviousMapTileLocation.GetValue());
	}

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



