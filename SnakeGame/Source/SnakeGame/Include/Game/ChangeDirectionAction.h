#pragma once

#include "ChangeDirectionAction.generated.h"

USTRUCT(BlueprintType)
struct FChangeDirectionAction
{
    GENERATED_BODY()
public:
    FVector Location{};
    FVector Direction{};
};