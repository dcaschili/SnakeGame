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
};