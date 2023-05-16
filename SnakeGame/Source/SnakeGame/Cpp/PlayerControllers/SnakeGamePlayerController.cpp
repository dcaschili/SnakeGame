#include "PlayerControllers/SnakeGamePlayerController.h"

#include "Pages/GDTUIUWBasePageLayout.h"
#include "Blueprint/UserWidget.h"
#include "SnakeLog.h"
#include "Engine.h"

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
	if (!DefaultLayoutPageClass)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Missing base layout page class! You need to setup a base page from .ini"));
		GDTUI_LOG(SnakeLogCategoryUI, Error, TEXT("Missing base layout page class! You need to setup a base page from Game.ini"));
		ensure(false);
		return;
	}

	TSubclassOf<UGDTUIUWBasePageLayout> TmpPageLayoutClass = DefaultLayoutPageClass;
	if (bOverrideDefaultLayoutPageClass)
	{
		if (!LayoutPageClassOverride)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("ERROR: wrong base layout page configuration."));
			GDTUI_LOG(SnakeLogCategoryUI, Error, TEXT("Layout override requested but missing layout class in configuration!"));
			ensure(false);
			return;
		}

		TmpPageLayoutClass = LayoutPageClassOverride;
	}

	if (ensure(TmpPageLayoutClass))
	{
		BaseLayoutPage = CreateWidget<UGDTUIUWBasePageLayout>(this, TmpPageLayoutClass);
		if (ensure(BaseLayoutPage))
		{
			BaseLayoutPage->AddToViewport();
		}
	}
}
