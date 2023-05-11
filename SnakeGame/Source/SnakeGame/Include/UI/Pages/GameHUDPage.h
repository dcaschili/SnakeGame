#pragma once

#include "Pages/GDTUIUWBasePage.h"
#include "Data/Model/GameDataModelUserInterface.h"

#include "GameHUDPage.generated.h"

class UGameDataModel;

UCLASS()
class SNAKEGAME_API UGameHUDPage : public UGDTUIUWBasePage, public IGameDataModelUserInterface
{
    GENERATED_BODY()

public:
	// Inherited via IGameDataModelUserInterface
	virtual void SetDataModel(UGameDataModel* InModel) override;
	virtual const UGameDataModel* GetDataModel() const override { return DataModel; }

	void SetHUDConfigurationActive(bool bInIsActive);

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Default)
	bool bHUDConfigurationActive = false;

private:
	virtual void InnerOnActivated() override;
	virtual void InnerOnDeactivated() override;

	UPROPERTY()
	UGameDataModel* DataModel{};

};