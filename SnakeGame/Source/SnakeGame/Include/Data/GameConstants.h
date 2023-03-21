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
		Map side length in Unreal units.
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Map", meta = (ClampMin = "5", UIMin = "5"))
	int32 MapSideLength = 10;
};