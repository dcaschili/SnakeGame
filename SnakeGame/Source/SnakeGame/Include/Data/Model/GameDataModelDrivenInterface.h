#pragma once

#include "GameDataModelDrivenInterface.generated.h"

class UGameDataModel;

UINTERFACE(meta=(CannotImplementInterfaceInBlueprint))
class SNAKEGAME_API UGameDataModelDrivenInterface : public UInterface
{
    GENERATED_BODY()
};

class SNAKEGAME_API IGameDataModelDrivenInterface
{
    GENERATED_BODY()
public:
    virtual void SetDataModel(UGameDataModel* InModel) = 0;
    virtual const UGameDataModel* GetDataModel() const = 0;
};