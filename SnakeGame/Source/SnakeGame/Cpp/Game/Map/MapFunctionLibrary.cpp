#include "Game/Map/MapFunctionLibrary.h"

#include "Game/Map/MapManager.h"
#include "Data/GameConstants.h"

bool UMapFunctionLibrary::GetMapTileFromWorldLocation(const UObject* InWorldContextObject, const FVector& InLocation, FIntVector2& OutTileLocation)
{
	AMapManager* const MapManager = AMapManager::GetMapManager(InWorldContextObject);


	const UGameConstants* const GameConstants = UGameConstants::GetGameConstants(InWorldContextObject);
	check(GameConstants);

	const int32 MapTileSize = GameConstants->TileSize;
	const float HalfMapTileSize = MapTileSize / 2.0f;
	const int32 MapSideTilesCount = GameConstants->MapSideTilesCount;
	const float HalfMapSideTilesCount = MapSideTilesCount / 2.0f;

	const int32 WorldSpaceIntRow = InLocation.X - FMath::Abs(FMath::Fmod(InLocation.X, MapTileSize));
	const int32 Row = (WorldSpaceIntRow / MapTileSize) + HalfMapSideTilesCount;

	const int32 WorldSpaceIntColumn = InLocation.Y - FMath::Abs(FMath::Fmod(InLocation.Y, MapTileSize));
	const int32 Column = (WorldSpaceIntColumn / MapTileSize) + HalfMapSideTilesCount;
	
	if (Row < 0 || Column < 0 || Row >= GameConstants->MapSideTilesCount || Column >= GameConstants->MapSideTilesCount)
	{		
		return false;
	}

	OutTileLocation.X = Column;
	OutTileLocation.Y = Row;

	return true;
}

bool UMapFunctionLibrary::GetWorldLocationFromTile(const UObject* InWorldContextObject, const FIntVector2& InTileLocation, FVector& OutLocation)
{
	const UGameConstants* const GameConstants = UGameConstants::GetGameConstants(InWorldContextObject);
	check(GameConstants);

	const int32 Row = InTileLocation.Y;
	const int32 Column = InTileLocation.X;

	const int32 MapSideTilesCount = GameConstants->MapSideTilesCount;
	if (Row >= MapSideTilesCount || Column >= MapSideTilesCount || Row < 0 || Column < 0)
	{
		return false;
	}

	const float HalfMapSideTilesCount = MapSideTilesCount / 2.0f;

	const int32 MapTileSize = GameConstants->TileSize;
	const float HalfMapTileSize = MapTileSize / 2.0f;

	// Considering map centered at the origin.
	float RowWorldMap = ((Row - HalfMapSideTilesCount) * MapTileSize) + HalfMapTileSize;
	float ColWorldMap = ((Column - HalfMapSideTilesCount) * MapTileSize) + HalfMapTileSize;
	OutLocation.X = RowWorldMap;
	OutLocation.Y = ColWorldMap;
	OutLocation.Z = 0.0f;


	return true;
}

bool UMapFunctionLibrary::IsWorldLocationNearCurrentTileCenter(const UObject* InWorldContextObject, const FVector& CurrentLocation)
{
	const UGameConstants* const GameConstants = UGameConstants::GetGameConstants(InWorldContextObject);
	check(GameConstants);

	const float CenterReachedPercentageTolerance = GameConstants->CenterReachedPercentageTolerance;
	const int32 TileSize = GameConstants->TileSize;
	const float HalfTileSize = TileSize / 2.0f;

	const float DistanceFromTileCenterTolerance = (HalfTileSize * CenterReachedPercentageTolerance) / 100.0f;

	// Convert to int
	FIntVector2 IntCurrentPos{};
	IntCurrentPos.X = FMath::RoundToInt(CurrentLocation.X);
	IntCurrentPos.Y = FMath::RoundToInt(CurrentLocation.Y);

	// Get position within tile sizes [0, TileSize]
	FIntVector2 TileSizeCurrentPos{};
	TileSizeCurrentPos.X = FMath::Abs(IntCurrentPos.X) % TileSize;
	TileSizeCurrentPos.Y = FMath::Abs(IntCurrentPos.Y) % TileSize;

	FVector ToPointInTile{};
	ToPointInTile.X = TileSizeCurrentPos.X - HalfTileSize;
	ToPointInTile.Y = TileSizeCurrentPos.Y - HalfTileSize;
	ToPointInTile.Z = 0.0f;

    return ToPointInTile.IsNearlyZero(DistanceFromTileCenterTolerance);
}

bool UMapFunctionLibrary::AlignWorldLocationToTileCenter(const UObject* InWorldContextObject, const FVector& InCurrentLocation, FVector& OutLocation)
{
	FIntVector2 TileLocation{};
	if (GetMapTileFromWorldLocation(InWorldContextObject, InCurrentLocation, TileLocation))
	{
		FVector WorldLocation{};
		if (GetWorldLocationFromTile(InWorldContextObject, TileLocation, WorldLocation))
		{
			OutLocation = MoveTemp(WorldLocation);
			return true;
		}
	}

	return false;
}
