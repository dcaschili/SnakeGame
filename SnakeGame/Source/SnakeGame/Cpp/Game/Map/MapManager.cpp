#include "Game/Map/MapManager.h"

#include "Data/GameConstants.h"
#include "Math/IntVector.h"
#include "SnakeLog.h"
#include "Kismet/GameplayStatics.h"
#include "SnakeGameGameModeBase.h"
#include "Game/Map/MapFunctionLibrary.h"

#if !UE_BUILD_SHIPPING

#include "Utils/GDTCDebugFunctionLibrary.h"

static TAutoConsoleVariable<bool> CVarEnableOccupancyDebug(
	TEXT("Snake.EnableOccupancyDebug"),
	false,
	TEXT("Enable drawing some debug box to visualize the occupancy for the tiles.\n"),
	ECVF_Cheat);
#endif // !UE_BUILD_SHIPPING

AMapManager::AMapManager()
	: Super()
{
	PrimaryActorTick.bCanEverTick = false;

#if !UE_BUILD_SHIPPING
	PrimaryActorTick.bCanEverTick = true;
#endif // !UE_BUILD_SHIPPING
}

AMapManager* AMapManager::GetMapManager(const UObject* InWorldContextObject)
{
	ASnakeGameGameModeBase* GameMode = Cast<ASnakeGameGameModeBase>(UGameplayStatics::GetGameMode(InWorldContextObject));
	// GameMode available only on server.
	return GameMode ? GameMode->GetMapManager() : nullptr;
}

bool AMapManager::GetRandomFreeMapLocation(FVector& OutWorldPosition) const
{
	if (!HasFreeLocation())
	{
		UE_LOG(SnakeLogCategoryMap, Warning, TEXT("Game tile maps is full! It shouldn't be possibile to reach this point!"));
		ensure(false);
		return false;
	}

	while (true)
	{
		int32 Row = FMath::RandRange(0, GetMapSideLength() - 1);
		int32 Col = FMath::RandRange(0, GetMapSideLength() - 1);

		if (IsMapLocationFree(Row, Col))
		{
			const float HalfMapSide = GetMapSideLength() / 2.0f;
			const float HalfTileSize = GetMapTileSize() / 2.0f;

			// Considering map centered at the origin.
			float RowWorldMap = ((Row - HalfMapSide) * GetMapTileSize()) + HalfTileSize;
			float ColWorldMap = ((Col - HalfMapSide) * GetMapTileSize()) + HalfTileSize;
			OutWorldPosition.X = RowWorldMap;
			OutWorldPosition.Y = ColWorldMap;
			OutWorldPosition.Z = 0.0f;
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
	return IsMapLocationFree(Pos.Y, Pos.X);
}

void AMapManager::IncrementTileOccupancy(const FIntVector2& Pos)
{
	int32 MapIndex = -1;
	if (ensure(GetMapTileIndex(Pos, MapIndex)))
	{
		AddTileOccupancy(MapIndex, 1);
		UE_LOG(SnakeLogCategoryMap, Verbose, TEXT("Incremented occupancy in tile Row: %d, Col %d. New total occupancy: %d"), Pos.Y, Pos.X, TilesItemsCount[MapIndex]);
	}
}

void AMapManager::ReduceTileOccupancy(const FIntVector2& Pos)
{
	int32 MapIndex = -1;
	if (ensure(GetMapTileIndex(Pos, MapIndex)))
	{
		AddTileOccupancy(MapIndex, -1);
		UE_LOG(SnakeLogCategoryMap, Verbose, TEXT("Reduced occupancy in tile Row: %d, Col %d. New total occupancy: %d"), Pos.Y, Pos.X, TilesItemsCount[MapIndex]);
	}
}

void AMapManager::BeginPlay()
{
	Super::BeginPlay();

	InitializeTileItemsCount();
}

#if !UE_BUILD_SHIPPING
void AMapManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CVarEnableOccupancyDebug.GetValueOnGameThread())
	{
		for (int32 Column = 0; Column < GetMapSideLength(); ++Column)
		{
			for (int32 Row = 0; Row < GetMapSideLength(); ++Row)
			{
				FVector WorldLocation{};
				if (ensure(UMapFunctionLibrary::GetWorldLocationFromTile(this, FIntVector2(Column, Row), WorldLocation)))
				{
					if (Column == 0 && Row == 0)
					{
						UGDTCDebugFunctionLibrary::DrawBoxDebug(GetWorld(), WorldLocation, FVector(1.0f, 1.0f, 1.0f) * (GetMapTileSize() / 2.0f), FColor::Blue);
					}
					else if (Column == GetMapSideLength() - 1 && Row == GetMapSideLength() - 1)
					{
						UGDTCDebugFunctionLibrary::DrawBoxDebug(GetWorld(), WorldLocation, FVector(1.0f, 1.0f, 1.0f) * (GetMapTileSize() / 2.0f), FColor::Orange);
					}
					else
					{
						if (GetItemsCountInTile(Row, Column) > 0)
						{
							UGDTCDebugFunctionLibrary::DrawBoxDebug(GetWorld(), WorldLocation, FVector(1.0f, 1.0f, 1.0f) * (GetMapTileSize() / 2.0f), FColor::Red);
						}
						else
						{
							//UGDTCDebugFunctionLibrary::DrawBoxDebug(GetWorld(), WorldLocation, FVector(1.0f, 1.0f, 1.0f) * (GetMapTileSize() / 2.0f));
						}
					}
					
				}
			}
		}
	}
}
#endif // !UE_BUILD_SHIPPING

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
	return GameConstants->MapSideTilesCount;
}

int32 AMapManager::GetMapTileSize() const
{
	const UGameConstants* GameConstants = UGameConstants::GetGameConstants(this);
	check(GameConstants);
	return GameConstants->TileSize;
}

int32 AMapManager::GetItemsCountInTile(int32 Row, int32 Column) const
{
	int32 MapIndex = -1;
	if (ensure(GetMapTileIndex(Row, Column, MapIndex)))
	{
		if (ensure(TilesItemsCount.IsValidIndex(MapIndex)))
		{
			return TilesItemsCount[MapIndex];
		}
	}

	return -1;
}

bool AMapManager::GetMapTileIndex(const FIntVector2& InPos, int32& OutIndex) const
{
	return GetMapTileIndex(InPos.Y, InPos.X, OutIndex);
}

bool AMapManager::GetMapTileIndex(int32 Row, int32 Column, int32& OutIndex) const
{
	const int32 MapIndex = Row * GetMapSideLength() + Column;
	if (TilesItemsCount.IsValidIndex(MapIndex))
	{
		OutIndex = MapIndex;
		return true;
	}

	return false;
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

void AMapManager::AddTileOccupancy(int32 MapIndex, int32 AmountToAdd)
{
	if (TilesItemsCount.IsValidIndex(MapIndex))
	{
		TilesItemsCount[MapIndex] += AmountToAdd;
		if (TilesItemsCount[MapIndex] < 0)
		{
			TilesItemsCount[MapIndex] = 0;
			UE_LOG(SnakeLogCategoryMap, Error, TEXT("Negative tile occupancy found!"));
			ensure(false);
		}
	}
}
