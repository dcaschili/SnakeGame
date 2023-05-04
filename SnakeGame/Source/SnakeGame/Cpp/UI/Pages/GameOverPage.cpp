#include "UI/Pages/GameOverPage.h"

void UGameOverPage::SetDataModel(UGameDataModel* InModel)
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
