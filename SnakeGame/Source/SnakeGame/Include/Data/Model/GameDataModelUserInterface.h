#pragma once

#include "GameDataModelUserInterface.generated.h"

class UGameDataModel;

UINTERFACE(meta=(CannotImplementInterfaceInBlueprint))
class SNAKEGAME_API UGameDataModelUserInterface : public UInterface
{
    GENERATED_BODY()
};

class SNAKEGAME_API IGameDataModelUserInterface
{
    GENERATED_BODY()
public:

    virtual void SetDataModel(UGameDataModel* InModel) = 0;
	UFUNCTION(BlueprintCallable)
	virtual const UGameDataModel* GetDataModel() const PURE_VIRTUAL(IGameDataModelUserInterface::GetDataModel, return nullptr;);
};