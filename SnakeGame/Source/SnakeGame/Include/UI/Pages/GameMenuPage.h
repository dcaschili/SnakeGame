#pragma once

#include "Pages/GDTUIUWSimpleButtonListPage.h"
#include "Data/Model/GameDataModelUserInterface.h"

#include "GameMenuPage.generated.h"

UCLASS()
class SNAKEGAME_API UGameMenuPage : public UGDTUIUWSimpleButtonListPage, public IGameDataModelUserInterface
{
    GENERATED_BODY()
    

public:
	// IGameDataModelUserInterface
	virtual void SetDataModel(UGameDataModel* InModel) override;
	virtual const UGameDataModel* GetDataModel() const override { return DataModel; }

private:
	UPROPERTY()
	UGameDataModel* DataModel {};
};