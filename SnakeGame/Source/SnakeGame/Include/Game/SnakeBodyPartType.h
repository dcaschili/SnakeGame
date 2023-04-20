#pragma once

#include "SnakeBodyPartType.generated.h"

UENUM(BlueprintType)
enum class ESnakeBodyPartType : uint8
{
    kNone UMETA(DisplayName="None"),
    kHead UMETA(DisplayName="Head"),
    kBody UMETA(DisplayName="Body"),
    kTail UMETA(DisplayName="Tail"),
};

