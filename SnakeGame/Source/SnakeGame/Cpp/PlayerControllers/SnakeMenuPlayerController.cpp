#include "PlayerControllers/SnakeMenuPlayerController.h"

#include "Pages/GDTUIUWSimpleButtonListPage.h"
#include "Pages/GDTUIUWBasePageLayout.h"
#include "Blueprint/UserWidget.h"
#include "SnakeLog.h"

#include "Engine.h"

void ASnakeMenuPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindEvents();
	Super::EndPlay(EndPlayReason);
}

void ASnakeMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeUIOnly InputModeUIOnly{};
	SetInputMode(InputModeUIOnly);

	// Setup layout and page
	if (BaseLayoutPage)
	{
		if (MenuPageClass)
		{
			ButtonListPage = Cast<UGDTUIUWSimpleButtonListPage>(BaseLayoutPage->PushWidget(MenuPageClass, EPageLayoutStackType::Main));
		}
		else
		{
			UE_LOG(SnakeLogCategoryUI, Error, TEXT("ASnakeMenuPlayerController - Missing Menu Page class"));
			ensure(false);
		}
	}

	BindEvents();
}

void ASnakeMenuPlayerController::BindEvents()
{
	if (ButtonListPage)
	{
		ButtonListPage->OnButtonClicked.AddUniqueDynamic(this, &ThisClass::HandleButtonClicked);
	}
}

void ASnakeMenuPlayerController::UnbindEvents()
{
	if (ButtonListPage)
	{
		ButtonListPage->OnButtonClicked.RemoveDynamic(this, &ThisClass::HandleButtonClicked);
	}
}

void ASnakeMenuPlayerController::HandleButtonClicked(const FName& InButtonId)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, *FString::Printf(TEXT("Button clicked: %s"), *InButtonId.ToString()));
	if (EMenuAction* Action = ButtonIdToMenuAction.Find(InButtonId))
	{
		switch (*Action)
		{
		case EMenuAction::kStart:
			break;
		case EMenuAction::kExit:
			UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
			break;
		default:
			break;
		}
	}
	else
	{
		FString Msg = FString::Printf(TEXT("Missing menu action for button: %s"), *InButtonId.ToString());
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, *Msg);
		UE_LOG(SnakeLogCategoryUI, Error, TEXT("%s"), *Msg);
		ensure(false);
	}


}
