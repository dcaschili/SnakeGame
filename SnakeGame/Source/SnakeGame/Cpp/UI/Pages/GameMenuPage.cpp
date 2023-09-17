#include "UI/Pages/GameMenuPage.h"

#include "Data/GDTUIDataModel.h"

void UGameMenuPage::SetDataModel(UGDTUIDataModel* InModel)
{
	if (InModel)
	{
		DataModel = InModel;
		Execute_RefreshUI(this);
	}
}

