#include "PlayerControllers/SnakeGamePlayerController.h"

#include "Pages/GDTUIUWBasePageLayout.h"
#include "Blueprint/UserWidget.h"
#include "SnakeLog.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"

#include "Engine.h"

void ASnakeGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (BaseControllerMappingContext)
	{
		const ULocalPlayer* const LP = GetLocalPlayer();
		UEnhancedInputLocalPlayerSubsystem* const EnhancedInputSubsystem = LP ? LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;
		if (EnhancedInputSubsystem)
		{
			EnhancedInputSubsystem->AddMappingContext(BaseControllerMappingContext, 0);
		}
	}
	else
	{
		GDTUI_PRINT_TO_SCREEN_ERROR(TEXT("Missing base controller mapping context in player controller class!"));
		ensure(false);
	}

	// Setup layout and page
	if (HasAuthority() && GetNetMode() != NM_DedicatedServer)
	{
		SetupBaseLayout();
	}
}

void ASnakeGamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (CloseGameAction)
	{
		if (UEnhancedInputComponent* const EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			EnhancedInputComponent->BindAction(CloseGameAction, ETriggerEvent::Triggered, this, &ThisClass::HandleCloseGameAction);
		}
	}
	else
	{
		GDTUI_PRINT_TO_SCREEN_ERROR(TEXT("Missing close game input action in player controller class!"));
		ensure(false);
	}
}

void ASnakeGamePlayerController::HandleCloseGameAction(const FInputActionInstance& InputInstance)
{
	GDTUI_SHORT_LOG(SnakeLogCategoryUI, Verbose, TEXT("Close Game Requested!"));
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
}

void ASnakeGamePlayerController::SetupBaseLayout()
{
	if (!DefaultLayoutPageClass)
	{
		GDTUI_PRINT_TO_SCREEN_ERROR(TEXT("Missing base layout page class! You need to setup a base page from .ini"));
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
