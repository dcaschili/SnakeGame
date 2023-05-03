#pragma once

#include "Data/Model/GameDataModel.h"

#include "PlayerScoreDataModel.generated.h"

UCLASS(BlueprintType)
class SNAKEGAME_API UPlayerScoreDataModel : public UGameDataModel
{
    GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "SnakeGame|DataModel")
	int32 CurrentScore = -1;
	UPROPERTY(BlueprintReadOnly, Category = "SnakeGame|DataModel")
	int32 BestScore = -1;
};