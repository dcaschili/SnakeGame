#pragma once

#include "Game/Snake/SnakeBodyPartType.h"

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

	virtual bool IsSnakeTail() const { return GetSnakeBodyPartType() == ESnakeBodyPartType::kTail; }
};
