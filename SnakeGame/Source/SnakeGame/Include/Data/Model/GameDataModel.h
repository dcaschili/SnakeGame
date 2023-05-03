#pragma once

#include "UObject/Object.h"

#include "GameDataModel.generated.h"

UCLASS()
class SNAKEGAME_API UGameDataModel : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly)
    FName Key{};
};