#include "PlayerControllers/SnakeGamePlayerController.h"

#include "Pages/GDTUIUWBasePageLayout.h"
#include "Blueprint/UserWidget.h"
#include "SnakeLog.h"

#if !UE_BUILD_SHIPPING
#include "Engine.h"
#endif

void ASnakeGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Setup layout and page
	if (HasAuthority() && GetNetMode() != NM_DedicatedServer)
	{
		SetupBaseLayout();
	}
}

void ASnakeGamePlayerController::SetupBaseLayout()
{
	if (BaseLayoutPageClass)
	{
		BaseLayoutPage = CreateWidget<UGDTUIUWBasePageLayout>(this, BaseLayoutPageClass);
		if (ensure(BaseLayoutPage))
		{
			BaseLayoutPage->AddToViewport();
		}
	}
	else
	{
#if !UE_BUILD_SHIPPING
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Missing base layout page class!"));
#endif // !UE_BUILD_SHIPPING

		UE_LOG(SnakeLogCategoryUI, Warning, TEXT("Missing base layout page class!"));
		ensure(false);
	}
}
