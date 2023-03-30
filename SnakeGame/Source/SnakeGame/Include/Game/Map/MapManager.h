#pragma once

#include "GameFramework/Actor.h"

#include "MapManager.generated.h"

UCLASS()
class SNAKEGAME_API AMapManager : public AActor
{
    GENERATED_BODY()
public:
	AMapManager();
	
	static AMapManager* GetMapManager(const UObject* InWorldContextObject);

	bool		GetRandomFreeMapLocation(FVector& OutWorldPosition) const;

	/**
		Check whether the tile position is free or empty
		@param Pos x: Column, y: Row
	*/
	bool		IsMapLocationFree(int32 Row, int32 Col) const;
	bool		IsMapLocationFree(const FIntVector2& Pos) const;
	void		IncrementTileOccupancy(const FIntVector2& Pos);
	void		ReduceTileOccupancy(const FIntVector2& Pos);

protected:
	virtual void BeginPlay() override;

#if !UE_BUILD_SHIPPING
	virtual void Tick(float DeltaSeconds);
#endif // !UE_BUILD_SHIPPING

private:	
	bool	HasFreeLocation() const;
	int32	GetMapSideLength() const;
	int32	GetMapTileSize() const;
	int32	GetItemsCountInTile(int32 Row, int32 Column) const;
	
	/* Column: InPos.X - Row: InPos.Y*/
	bool	GetMapTileIndex(const FIntVector2& InPos, int32& OutIndex) const;
	bool	GetMapTileIndex(int32 Row, int32 Column, int32& OutIndex) const;

	void	InitializeTileItemsCount();

	void	AddTileOccupancy(int32 MapIndex, int32 AmountToAdd);

	TArray<int32> TilesItemsCount{};
};