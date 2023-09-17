#pragma once

#include "Pages/GDTUIUWSimpleButtonListPage.h"
#include "Interfaces/GDTUIDataModelInterface.h"

#include "GameOverPage.generated.h"

class UGDTUIDataModel;

UCLASS()
class SNAKEGAME_API UGameOverPage : public UGDTUIUWSimpleButtonListPage, public IGDTUIDataModelInterface
{
    GENERATED_BODY()
public:

	// Inherited via IGDTUIDataModelInterface
	virtual void SetDataModel(UGDTUIDataModel* InModel) override;
	virtual const UGDTUIDataModel* GetDataModel() const override { return DataModel; }

protected:
	virtual void InnerOnActivated() override;
	virtual void InnerOnDeactivated() override;

private:
	UPROPERTY()
	UGDTUIDataModel* DataModel{};
};