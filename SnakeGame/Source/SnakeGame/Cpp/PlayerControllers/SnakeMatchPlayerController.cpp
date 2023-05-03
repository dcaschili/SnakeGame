#include "PlayerControllers/SnakeMatchPlayerController.h"

#include "Pages/GDTUIUWBasePage.h"
#include "SnakeLog.h"
#include "CommonActivatableWidget.h"
#include "Pages/GDTUIUWBasePageLayout.h"

#if !UE_BUILD_SHIPPING
#include "Engine.h"
#endif // !UE_BUILD_SHIPPING

void ASnakeMatchPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputModeGameOnly{};
	SetInputMode(InputModeGameOnly);

	if (BaseLayoutPage)
	{
		if (HUDMatchPageClass)
		{
			HUDPage = Cast<UGDTUIUWBasePage>(BaseLayoutPage->PushWidget(HUDMatchPageClass, EPageLayoutStackType::GameUI));

			// TODO: Setup data model
		}
		else
		{
#if !UE_BUILD_SHIPPING
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("ASnakeMatchPlayerController - Missing HUD page class!"));
#endif // !UE_BUILD_SHIPPING
			UE_LOG(SnakeLogCategoryUI, Warning, TEXT("ASnakeMatchPlayerController - Missing HUD page class!"));
			ensure(false);
		}
	}
}
