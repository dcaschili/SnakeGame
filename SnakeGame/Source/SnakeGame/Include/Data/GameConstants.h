#pragma once

#include "Engine/DataAsset.h"

#include "GameConstants.generated.h"

UCLASS()
class SNAKEGAME_API UGameConstants : public UDataAsset
{
    GENERATED_BODY()
public:
    /**
     * @brief Size of a tile of the map in unreal unit.     
     */
    UPROPERTY(EditDefaultsOnly, Category="Map", meta=(ClampMin="0", UIMin="0"))
    int32 TileSize = 100;
};