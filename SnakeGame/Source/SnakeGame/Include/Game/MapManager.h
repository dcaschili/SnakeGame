#pragma once

#include "GameFramework/Actor.h"

#include "MapManager.generated.h"

UCLASS()
class SNAKEGAME_API AMapManager : public AActor
{
    GENERATED_BODY()
public:

	bool		GetRandomFreeMapLocation(int32& OutRow, int32& OutCol) const;

	/**
		Check whether the tile position is free or empty
		@param Pos x: Row, y: Column
	*/
	bool		IsMapLocationFree(int32 Row, int32 Col) const;
	bool		IsMapLocationFree(const FIntVector2& Pos) const;

protected:
	virtual void BeginPlay() override;

private:	
	bool	HasFreeLocation() const;
	int32	GetMapSideLength() const;
	int32	GetItemsCountInTile(int32 Row, int32 Column) const;

	void	InitializeTileItemsCount();

	TArray<int32> TilesItemsCount{};
};