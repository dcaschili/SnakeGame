#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "MapFunctionLibrary.generated.h"

UCLASS()
class SNAKEGAME_API UMapFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
	/**
		Converts a world space location into a tile on the map manager.
		@return true if a location exists, false otherwise. 
	*/
	static bool GetMapTileFromWorldLocation(const UObject* InWorldContextObject, const FVector& InLocation, FIntVector2& OutTileLocation);
	
	/**
		Converts a tile map location into a world location if the map location exists.
		@param InWorldContextObject
		@param InTileLocation Y -> Row; X -> Column.
		@return true if a location exists, false otherwise. 
	*/
	static bool GetWorldLocationFromTile(const UObject* InWorldContextObject, const FIntVector2& InTileLocation, FVector& OutLocation);	

	/** 
		It checks whether the CurrentLocation is at the tile
		center or on the "outer border".
		In the game constants a tolerance value for the
		tile center check is defined.
		@param InWorldContextObject
		@param CurrentLocation The current location
		@return true if the location is on the center (considering tolerance), false otherwise.
	*/
	static bool IsWorldLocationNearCurrentTileCenter(const UObject* InWorldContextObject, const FVector& CurrentLocation);

	/**
		It converts a world location to the nearest tile center, if it is possible.
		@param InWorldContextObject
		@param InCurrentLocation	The current location
		@param OutLocation			The center of the tile location
		@return true if the conversion was successful
	*/
	static bool AlignWorldLocationToTileCenter(const UObject* InWorldContextObject, const FVector& InCurrentLocation, FVector& OutLocation);

	/**
		Given a location and a vector, it returns the next tile in the map following that direction.
		@param InWorldContextObject
		@param InCurrentPos		The world position used to get the current tile.
		@param InCurrentDir		The direction used to find the next tile.		
		@param OutNextTile		The next tile
		@return	true if a next tile is available within the map, false otherwise
	*/
	static bool GetFollowingTile(const UObject* InWorldContextObject, const FVector& InCurrentPos, const FVector& InCurrentDir, FVector& OutNextTile);
	
	/**
		It checks if the PositionToCheck, reached with the given InDirection, has passed the Target
		or not. 
		@param InWorldContextObject
		@param InPositionToCheck	The world position to check agains the target.
		@param InDirection			The direction followed to reach the PositionToCheck.		
		@param InTarget				The location to check.
		@return		true if overshoot, false otherwise
	*/
	static bool DoesOvershootPosition(const UObject* InWorldContextObject, const FVector& InPositionToCheck, const FVector& InDirection, const FVector& InTarget);
};