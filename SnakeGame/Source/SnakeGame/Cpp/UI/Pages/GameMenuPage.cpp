#include "UI/Pages/GameMenuPage.h"

void UGameMenuPage::SetDataModel(UGameDataModel* InModel)
{
	if (InModel)
	{
		DataModel = InModel;
		Execute_RefreshUI(this);
	}
}

