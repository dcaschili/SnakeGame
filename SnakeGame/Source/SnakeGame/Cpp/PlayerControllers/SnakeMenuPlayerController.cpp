#include "PlayerControllers/SnakeMenuPlayerController.h"

#include "UI/Pages/GameMenuPage.h"
#include "Pages/GDTUIUWBasePageLayout.h"
#include "Blueprint/UserWidget.h"
#include "SnakeLog.h"
#include "SnakeGameLocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Data/PlayerProfileSaveGame.h"
#include "Interfaces/GDTUIDataModelInterface.h"
#include "Data/Model/PlayerScoreDataModel.h"

#include "Engine.h"

void ASnakeMenuPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindEvents();
	Super::EndPlay(EndPlayReason);
}

void ASnakeMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// load savegame.
	USnakeGameLocalPlayer* const SnakeGameLocalPlayer = Cast<USnakeGameLocalPlayer>(GetLocalPlayer());
	if (SnakeGameLocalPlayer && SnakeGameLocalPlayer->GetNeedsSaveGameLoad())
	{
		GDTUI_SHORT_LOG(SnakeLogCategorySave, Log, TEXT("Loading Player profile savegame."));
		UPlayerProfileSaveGame* ProfileSaveGame = Cast<UPlayerProfileSaveGame>(UGameplayStatics::LoadGameFromSlot(UPlayerProfileSaveGame::PlayerProfileSlotName, 0));
		if (!ProfileSaveGame)
		{
			GDTUI_LOG(SnakeLogCategorySave, Log, TEXT("Can't find player profile savegame! Creating a new savegame file!"));
			ProfileSaveGame = Cast<UPlayerProfileSaveGame>(UGameplayStatics::CreateSaveGameObject(UPlayerProfileSaveGame::StaticClass()));
		}

		SnakeGameLocalPlayer->SetPlayerProfileSaveGame(ProfileSaveGame);
	}


	FInputModeGameAndUI InputModeGameAndUI{};	
	InputModeGameAndUI.SetLockMouseToViewportBehavior(EMouseLockMode::LockOnCapture);
	SetInputMode(InputModeGameAndUI);
	SetShowMouseCursor(true);

	// Setup layout and page
	if (BaseLayoutPage)
	{
		if (MenuPageClass)
		{
			if (IGDTUIDataModelInterface* const Interface = Cast<IGDTUIDataModelInterface>(BaseLayoutPage->PushWidget(MenuPageClass, EPageLayoutStackType::Main)))
			{
				UPlayerScoreDataModel* const ScoreDataModel = NewObject<UPlayerScoreDataModel>();
				ScoreDataModel->BestScore = SnakeGameLocalPlayer ? SnakeGameLocalPlayer->GetBestScore() : -1;

				Interface->SetDataModel(ScoreDataModel);
			}
			else
			{
				GDTUI_LOG(SnakeLogCategoryUI, Error, TEXT("Some problem creating the menu page!"));
				ensure(false);
			}
		}
		else
		{
			GDTUI_LOG(SnakeLogCategoryUI, Error, TEXT("Missing Menu Page class"));
			ensure(false);
		}
	}

	BindEvents();
}

void ASnakeMenuPlayerController::BindEvents()
{
	if (BaseLayoutPage)
	{
		if (UGDTUIUWSimpleButtonListPage* const Page = Cast<UGDTUIUWSimpleButtonListPage>(BaseLayoutPage->GetActiveWidget(EPageLayoutStackType::Main)))
		{
			Page->OnButtonClicked.AddUniqueDynamic(this, &ThisClass::HandleButtonClicked);
		}
	}
}

void ASnakeMenuPlayerController::UnbindEvents()
{
	if (BaseLayoutPage)
	{
		if (UGDTUIUWSimpleButtonListPage* const Page = Cast<UGDTUIUWSimpleButtonListPage>(BaseLayoutPage->GetActiveWidget(EPageLayoutStackType::Main)))
		{
			Page->OnButtonClicked.RemoveDynamic(this, &ThisClass::HandleButtonClicked);
		}
	}
}

void ASnakeMenuPlayerController::HandleButtonClicked(const FName& InButtonId)
{
	GDTUI_PRINT_TO_SCREEN_LOG(TEXT("Button clicked: %s"), *InButtonId.ToString());
	if (EMenuAction* Action = ButtonIdToMenuAction.Find(InButtonId))
	{
		switch (*Action)
		{
		case EMenuAction::kStart:
			ClientTravel(FString(TEXT("/Game/Maps/Match")), TRAVEL_Absolute);
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
		const FString Msg = FString::Printf(TEXT("Missing menu action for button: %s"), *InButtonId.ToString());
		GDTUI_PRINT_TO_SCREEN_ERROR(TEXT("%s"), *Msg);
		GDTUI_LOG(SnakeLogCategoryUI, Error, TEXT("%s"), *Msg);
		ensure(false);
	}


}
