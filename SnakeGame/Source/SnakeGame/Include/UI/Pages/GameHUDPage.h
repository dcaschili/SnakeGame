#pragma once

#include "Pages/GDTUIUWBasePage.h"
#include "Data/Model/GameDataModelDrivenInterface.h"

#include "GameHUDPage.generated.h"

class UGameDataModel;

UCLASS()
class SNAKEGAME_API UGameHUDPage : public UGDTUIUWBasePage, public IGameDataModelDrivenInterface
{
    GENERATED_BODY()

public:
	// Inherited via IGameDataModelDrivenInterface
	virtual void SetDataModel(UGameDataModel* InModel) override;
	virtual const UGameDataModel* GetDataModel() const override { return DataModel; }

private:
	virtual void InnerOnActivated() override;
	virtual void InnerOnDeactivated() override;

	UPROPERTY()
	UGameDataModel* DataModel{};

};