#include "UI/Pages/GameOverPage.h"

#include "Data/GDTUIDataModel.h"

void UGameOverPage::SetDataModel(UGDTUIDataModel* InModel)
{
	if (InModel)
	{
		DataModel = InModel;
		Execute_RefreshUI(this);
	}
}

void UGameOverPage::InnerOnActivated()
{
	Super::InnerOnActivated();
	Execute_RefreshUI(this);
}

void UGameOverPage::InnerOnDeactivated()
{
	Super::InnerOnDeactivated();
}
