#pragma once

#include "Pages/GDTUIUWBasePage.h"
#include "Interfaces/GDTUIDataModelInterface.h"

#include "GameHUDPage.generated.h"

class UGDTUIDataModel;

UCLASS()
class SNAKEGAME_API UGameHUDPage : public UGDTUIUWBasePage, public IGDTUIDataModelInterface
{
    GENERATED_BODY()

public:
	// Inherited via IGameDataModelUserInterface
	virtual void SetDataModel(UGDTUIDataModel* InModel) override;
	virtual const UGDTUIDataModel* GetDataModel() const override { return DataModel; }

	void SetHUDConfigurationActive(bool bInIsActive);

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Default)
	bool bHUDConfigurationActive = false;

private:
	virtual void InnerOnActivated() override;
	virtual void InnerOnDeactivated() override;

	UPROPERTY()
	UGDTUIDataModel* DataModel{};

};