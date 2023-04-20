#pragma once

#include "Engine/DataAsset.h"

#include "GameConstants.generated.h"

UCLASS()
class SNAKEGAME_API UGameConstants : public UDataAsset
{
    GENERATED_BODY()
public:

	static const UGameConstants* GetGameConstants(const UObject* InWorldContextObject);

    /**
		Size of a tile of the map in Unreal units.
     */
    UPROPERTY(EditDefaultsOnly, Category="Map", meta=(ClampMin="1", UIMin="1"))
    int32 TileSize = 100;

	/**
		Number of tiles that composes a side of the map.
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Map", meta = (ClampMin = "5", UIMin = "5"))
	int32 MapSideTilesCount = 10;

	/**
		The change in direction is applied only when the snake
		reaches the center of a tile. This value represents
		the tolerance error from the center of the tile.
		The pawn is considered on the center of the tile if it
		has a distance smaller that this percentage amount of the
		half tile size.
		For example the calculation with HalfTileSize 50:
		Tolerance = (50 * this) / 100.0f;
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Movement", meta = (UIMin = "1.0", ClampMin = "1.0", UIMax = "100.0", ClampMax = "100.0"))
	float CenterReachedPercentageTolerance = 5.0f;
};