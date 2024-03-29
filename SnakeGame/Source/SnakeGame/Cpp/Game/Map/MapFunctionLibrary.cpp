#include "Game/Map/MapFunctionLibrary.h"

#include "Game/Map/MapManager.h"
#include "Data/GameConstants.h"
#include "SnakeLog.h"

bool UMapFunctionLibrary::GetMapTileFromWorldLocation(const UObject* InWorldContextObject, const FVector& InLocation, FIntVector2& OutTileLocation)
{
	const UGameConstants* const GameConstants = UGameConstants::GetGameConstants(InWorldContextObject);
	check(GameConstants);

	const int32 WorldTileSize = GameConstants->TileSize;
	const float HalfWorldTileSize = WorldTileSize / 2.0f;
	const int32 WorldSize = WorldTileSize * GameConstants->MapSideTilesCount;
	const float HalfWorldSize = WorldSize / 2.0f;

	const int32 MapTileSize = 1;
	const float HalfMapTileSize = MapTileSize / 2.0f;
	const int32 MapSize = GameConstants->MapSideTilesCount;
	const float HalfMapSize = MapSize / 2.0f;

	/*
		Map is centered at [0, 0] but the origin in terms of map tile is the bottom left corner.
		I have to map from world space to [0, 1] coordinates.
		By adding half the world map size I move from [-Half, +Half] to [0, 2Half] and then dividing by
		2Half I get the mapping [0, 1].
	*/	
	const float WorldXNormalized = (InLocation.X + HalfWorldSize) / WorldSize; // [0.0; 1.0]
	ensure(WorldXNormalized >= 0.0 && WorldXNormalized <= 1.0);
	// Remap in map space
	const float MapXPosition = (WorldXNormalized * MapSize) - HalfMapTileSize; // [-HalfMapTileSize; MapSize - HalfMapTileSize] -> es -0.5; 9.5
	ensure(MapXPosition >= -HalfMapTileSize && MapXPosition <= (MapSize - HalfMapTileSize));
	// Round to take the center of the tile.
	const int32 Row = FMath::RoundToInt(MapXPosition);


	// Move from world space to [0.0, 1.0] coordinates.
	const float WorldYNormalized = (InLocation.Y + HalfWorldSize) / WorldSize; // [0.0; 1.0]
	ensure(WorldYNormalized >= 0.0 && WorldYNormalized <= 1.0);
	// Remap in map space
	const float MapYPosition = (WorldYNormalized * MapSize) - HalfMapTileSize; // [-HalfMapTileSize; MapSize - HalfMapTileSize] -> es -0.5; 9.5
	ensure(MapYPosition >= -HalfMapTileSize && MapYPosition <= (MapSize - HalfMapTileSize));
	// Round to take the center of the tile.
	const int32 Col = FMath::RoundToInt(MapYPosition);

	OutTileLocation.X = Col;
	OutTileLocation.Y = Row;

	if (Row < 0 || Col < 0 || Row >= GameConstants->MapSideTilesCount || Col >= GameConstants->MapSideTilesCount)
	{
		return false;
	}

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

	FVector TileCenter{};
	if (ensure(AlignWorldLocationToTileCenter(InWorldContextObject, CurrentLocation, TileCenter)))
	{
		FVector ToPointInTile = TileCenter - CurrentLocation;
		ToPointInTile.Z = 0.0f;
		return ToPointInTile.IsNearlyZero(DistanceFromTileCenterTolerance);
	}

	return false;
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

bool UMapFunctionLibrary::GetFollowingTile(const UObject* InWorldContextObject, const FVector& InCurrentPos, const FVector& InCurrentDir, FVector& OutNextTile)
{
	// InTileLocation Y->Row; X->Column.
	FIntVector2 TileLocation{};
	if (GetMapTileFromWorldLocation(InWorldContextObject, InCurrentPos, TileLocation))
	{
		if (FMath::IsNearlyZero(InCurrentDir.X))
		{
			TileLocation.X += 1 * FMath::Sign(InCurrentDir.Y);			
		}
		else if (FMath::IsNearlyZero(InCurrentDir.Y))
		{
			TileLocation.Y += 1 * FMath::Sign(InCurrentDir.X);
		}
		else
		{
			GDTUI_LOG(SnakeLogCategoryMap, Warning, TEXT("Requested direction isn't valid! %s"), *InCurrentDir.ToString());
			return false;
		}

		return GetWorldLocationFromTile(InWorldContextObject, TileLocation, OutNextTile);
	}
	
	GDTUI_LOG(SnakeLogCategoryMap, Warning, TEXT("Requested position isn't valid! %s"), *InCurrentPos.ToString());
    return false;
}

bool UMapFunctionLibrary::DoesOvershootPosition(const UObject* InWorldContextObject, const FVector& InPositionToCheck, const FVector& InDirection, const FVector& InTarget)
{
	const FVector ToPosition = InPositionToCheck - InTarget;
	if (InDirection.Dot(ToPosition) > 0)
	{
		// Vector are pointing on the same direction, overshoot.
		return true;
	}

	return false;
}
