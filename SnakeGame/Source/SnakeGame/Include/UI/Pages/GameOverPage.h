#pragma once

#include "Pages/GDTUIUWSimpleButtonListPage.h"
#include "Data/Model/GameDataModelDrivenInterface.h"

#include "GameOverPage.generated.h"

class UPlayerScoreDataModel;

UCLASS()
class SNAKEGAME_API UGameOverPage : public UGDTUIUWSimpleButtonListPage, public IGameDataModelDrivenInterface
{
    GENERATED_BODY()
public:

	// Inherited via IGameDataModelDrivenInterface
	virtual void SetDataModel(UGameDataModel* InModel) override;
	virtual const UGameDataModel* GetDataModel() const override { return DataModel; }

protected:
	virtual void InnerOnActivated() override;
	virtual void InnerOnDeactivated() override;

private:
	UPROPERTY()
	UGameDataModel* DataModel{};
};