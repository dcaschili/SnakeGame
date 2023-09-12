#include "SnakeGameInstance.h"

#include "SnakeLog.h"
#include "HAL/IConsoleManager.h"

void USnakeGameInstance::Init()
{
	Super::Init();

	IConsoleVariable* CVarGlobalInvalidation = IConsoleManager::Get().FindConsoleVariable(TEXT("Slate.EnableGlobalInvalidation"));
	if (CVarGlobalInvalidation)
	{
		GDTUI_SHORT_LOG(SnakeLogCategoryUI, Log, TEXT("Enabling slate global invalidation!"));
		CVarGlobalInvalidation->Set(1);
	}

}
