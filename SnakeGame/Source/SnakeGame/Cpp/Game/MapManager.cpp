#include "Game/MapManager.h"

#include "Data/GameConstants.h"
#include "Math/IntVector.h"
#include "SnakeLog.h"

bool AMapManager::GetRandomFreeMapLocation(int32& OutRow, int32& OutCol) const
{
	if (!HasFreeLocation())
	{
		UE_LOG(SnakeLogCategoryMap, Warning, TEXT("Game tile maps is full! It shouldn't be possibile to reach this point!"));
		ensure(false);
		return false;
	}

	while (true)
	{
		int32 Row = FMath::RandRange(0, GetMapSideLength());
		int32 Col = FMath::RandRange(0, GetMapSideLength());

		if (IsMapLocationFree(Row, Col))
		{
			OutRow = Row;
			OutCol = Col;
			return true;
		}
	}

	return false;
}

bool AMapManager::IsMapLocationFree(int32 Row, int32 Col) const
{
	return GetItemsCountInTile(Row, Col) <= 0;
}

bool AMapManager::IsMapLocationFree(const FIntVector2& Pos) const
{
	return IsMapLocationFree(Pos.X, Pos.Y);
}

void AMapManager::BeginPlay()
{
	Super::BeginPlay();

	InitializeTileItemsCount();
}

bool AMapManager::HasFreeLocation() const
{
	for (int32 ItemsCount : TilesItemsCount)
	{
		if (ItemsCount <= 0) return true;
	}

	return false;
}

int32 AMapManager::GetMapSideLength() const
{
	const UGameConstants* GameConstants = UGameConstants::GetGameConstants(this);
	check(GameConstants);
	return GameConstants->MapSideLength;
}

int32 AMapManager::GetItemsCountInTile(int32 Row, int32 Column) const
{
	const int32 MapIndex = Row * GetMapSideLength() + Column;
	if (ensure(TilesItemsCount.IsValidIndex(MapIndex)))
	{
		return TilesItemsCount[MapIndex];
	}

	return -1;
}

void AMapManager::InitializeTileItemsCount()
{
	TilesItemsCount.Empty();

	const int32 MapTilesCount = GetMapSideLength() * GetMapSideLength();
	TilesItemsCount.Reserve(MapTilesCount);

	for (int32 Index = 0; Index < MapTilesCount; ++Index)
	{
		TilesItemsCount.Emplace(0);
	}
}
