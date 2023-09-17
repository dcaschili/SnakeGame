#pragma once

#include "Pages/GDTUIUWSimpleButtonListPage.h"
#include "Interfaces/GDTUIDataModelInterface.h"

#include "GameMenuPage.generated.h"

class UGDTUIDataModel;

UCLASS()
class SNAKEGAME_API UGameMenuPage : public UGDTUIUWSimpleButtonListPage, public IGDTUIDataModelInterface
{
    GENERATED_BODY()
    

public:
	// IGDTUIDataModelInterface
	virtual void SetDataModel(UGDTUIDataModel* InModel) override;
	virtual const UGDTUIDataModel* GetDataModel() const override { return DataModel; }

private:
	UPROPERTY()
	UGDTUIDataModel* DataModel {};
};