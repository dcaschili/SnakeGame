#include "UI/Pages/GameHUDPage.h"

#include "Data/GDTUIDataModel.h"

void UGameHUDPage::SetDataModel(UGDTUIDataModel* InModel)
{
	if (InModel)
	{
		DataModel = InModel;

		Execute_RefreshUI(this);
	}
}

void UGameHUDPage::SetHUDConfigurationActive(bool bInIsActive)
{
	bHUDConfigurationActive = bInIsActive;

	Execute_RefreshUI(this);
}

void UGameHUDPage::InnerOnActivated()
{
	Super::InnerOnActivated();

	Execute_RefreshUI(this);
}

void UGameHUDPage::InnerOnDeactivated()
{
	Super::InnerOnDeactivated();
}
