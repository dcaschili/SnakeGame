#pragma once

#include "Game/SnakeBodyPartType.h"

#include "SnakeBodyPartTypeInterface.generated.h"

UINTERFACE(meta=(CannotImplementInterfaceInBlueprint))
class SNAKEGAME_API USnakeBodyPartTypeInterface : public UInterface
{
    GENERATED_BODY()
};

class SNAKEGAME_API ISnakeBodyPartTypeInterface
{
    GENERATED_BODY()
public:
    virtual void SetSnakeBodyPartType(ESnakeBodyPartType InBodyPartType) = 0;
    virtual ESnakeBodyPartType GetSnakeBodyPartType() const = 0;
};
