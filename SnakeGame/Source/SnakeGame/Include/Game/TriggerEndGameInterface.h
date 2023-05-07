#pragma once

#include "CoreMinimal.h"

#include "TriggerEndGameInterface.generated.h"

UINTERFACE(meta=(CannotImplementInterfaceInBlueprint))
class SNAKEGAME_API UTriggerEndGameInterface : public UInterface
{
    GENERATED_BODY()
};

class SNAKEGAME_API ITriggerEndGameInterface
{
    GENERATED_BODY()
public:
    virtual bool ShouldTriggerEndGame() const { return true; }
};