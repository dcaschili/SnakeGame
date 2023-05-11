#pragma once

#include "Pages/GDTUIUWSimpleButtonListPage.h"
#include "Data/Model/GameDataModelUserInterface.h"

#include "GameOverPage.generated.h"

class UPlayerScoreDataModel;

UCLASS()
class SNAKEGAME_API UGameOverPage : public UGDTUIUWSimpleButtonListPage, public IGameDataModelUserInterface
{
    GENERATED_BODY()
public:

	// Inherited via IGameDataModelUserInterface
	virtual void SetDataModel(UGameDataModel* InModel) override;
	virtual const UGameDataModel* GetDataModel() const override { return DataModel; }

protected:
	virtual void InnerOnActivated() override;
	virtual void InnerOnDeactivated() override;

private:
	UPROPERTY()
	UGameDataModel* DataModel{};
};