#pragma once

#include "Data/GDTUIDataModel.h"

#include "PlayerScoreDataModel.generated.h"

class ASnakeGamePlayerState;

UCLASS(BlueprintType)
class SNAKEGAME_API UPlayerScoreDataModel : public UGDTUIDataModel
{
    GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, Category = "SnakeGame|DataModel")
	int32 CurrentScore = -1;
	UPROPERTY(BlueprintReadOnly, Category = "SnakeGame|DataModel")
	int32 BestScore = -1;
	UPROPERTY(BlueprintReadOnly, Category = "SnakeGame|DataModel")
	bool bIsBestScore = false;
};