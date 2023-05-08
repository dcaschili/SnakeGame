#include "PlayerControllers/SnakeMatchPlayerController.h"

#include "Pages/GDTUIUWBasePage.h"
#include "SnakeLog.h"
#include "CommonActivatableWidget.h"
#include "Pages/GDTUIUWBasePageLayout.h"
#include "SnakeGamePlayerState.h"
#include "Data/Model/PlayerScoreDataModel.h"
#include "Data/Model/GameDataModelDrivenInterface.h"
#include "UI/Pages/GameOverPage.h"
#include "Net/UnrealNetwork.h"
#include "SnakeMatchGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "SnakeGameLocalPlayer.h"

#if !UE_BUILD_SHIPPING
#include "Engine.h"
#endif // !UE_BUILD_SHIPPING

void ASnakeMatchPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ASnakeGamePlayerState* const SnakeGamePlayerState = Cast<ASnakeGamePlayerState>(PlayerState))
	{
		SnakeGamePlayerState->OnPlayerStateScoreUpdated.RemoveDynamic(this, &ThisClass::HandleScoreChanged);
	}

	if (ASnakeMatchGameModeBase* const SnakeMatchGameMode = Cast<ASnakeMatchGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		// Only on the server
		SnakeMatchGameMode->OnEndGame.RemoveDynamic(this, &ThisClass::HandleEndGameDelegate);
	}

	if (BaseLayoutPage)
	{
		UCommonActivatableWidget* const ActiveWidget = BaseLayoutPage->GetActiveWidget(EPageLayoutStackType::GameUI);
		if (UGameOverPage* const GameOverPage = Cast<UGameOverPage>(ActiveWidget))
		{
			GameOverPage->OnButtonClicked.RemoveDynamic(this, &ThisClass::HandleEndGamePageButtonClicked);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ASnakeMatchPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputModeGameOnly{};	
	SetInputMode(InputModeGameOnly);
	SetShowMouseCursor(false);

	if (ASnakeGamePlayerState* const SnakeGamePlayerState = Cast<ASnakeGamePlayerState>(PlayerState))
	{
		SnakeGamePlayerState->OnPlayerStateScoreUpdated.AddUniqueDynamic(this, &ThisClass::HandleScoreChanged);
	}

	if (BaseLayoutPage)
	{
		if (HUDMatchPageClass)
		{
			BaseLayoutPage->PushWidget(HUDMatchPageClass, EPageLayoutStackType::GameUI);

			UpdatePageScore();
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

	// Register to endgame event.
	if (ASnakeMatchGameModeBase* const SnakeMatchGameMode = Cast<ASnakeMatchGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		// Only on the server
		SnakeMatchGameMode->OnEndGame.AddUniqueDynamic(this, &ThisClass::HandleEndGameDelegate);
	}
}

void ASnakeMatchPlayerController::Multicast_EndGame_Implementation()
{
	GDTUI_LOG(SnakeLogCategoryGame, Verbose, TEXT("Received EndGame from server"));
	InnerHandleEndGame();
}

void ASnakeMatchPlayerController::HandleEndGamePageButtonClicked(const FName& InButtonId)
{
	GDTUI_LOG(SnakeLogCategoryUI, VeryVerbose, TEXT("EndGame page button clicked: %s"), *InButtonId.ToString());
	if (ensure(!GameOverPageContinueButtonId.IsNone()) && InButtonId.IsEqual(GameOverPageContinueButtonId))
	{
		// Handle savegame
		if (USnakeGameLocalPlayer* const SnakeLocalPlayer = Cast<USnakeGameLocalPlayer>(GetLocalPlayer()))
		{
			if(PlayerState)
			{
				SnakeLocalPlayer->UpdatePlayerScore(PlayerState->GetScore());
				SnakeLocalPlayer->SaveGame();
			}
		}

		// Go back to main menu
		ClientTravel(FString(TEXT("Game/Maps/Menu")), TRAVEL_Absolute);
	}
}

void ASnakeMatchPlayerController::HandleEndGameDelegate()
{
	GDTUI_LOG(SnakeLogCategoryGame, Verbose, TEXT("Received endgame delegate event on the server!"));
	
	// Forward event to all client.
	Multicast_EndGame();
}

void ASnakeMatchPlayerController::InnerHandleEndGame()
{
	// If on the client and has authority, show the end game page
	if (GetNetMode() != NM_DedicatedServer && HasAuthority())
	{
		FInputModeUIOnly InputModeUIOnly{};
		SetInputMode(InputModeUIOnly);
		SetShowMouseCursor(true);

		UE_LOG(SnakeLogCategoryGame, Verbose, TEXT("ASnakeMatchPlayerController - Show EndGame page on client!"));

		if (GameOverPageClass)
		{
			if (ensure(BaseLayoutPage))
			{
				UGameOverPage* const GameOverPage = Cast<UGameOverPage>(BaseLayoutPage->PushWidget(GameOverPageClass, EPageLayoutStackType::GameUI));
				if (GameOverPage)
				{
					// Record to event
					GameOverPage->OnButtonClicked.AddUniqueDynamic(this, &ThisClass::HandleEndGamePageButtonClicked);

					// Setup model.
					GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Setup endgame page data model!"));
				}
			}
		}
		else
		{
			UE_LOG(SnakeLogCategoryUI, Warning, TEXT("ASnakeMatchPlayerController - Missing game over class!"));
			ensure(false);
		}
	}
}

void ASnakeMatchPlayerController::HandleScoreChanged()
{
	UpdatePageScore();
}

void ASnakeMatchPlayerController::UpdatePageScore()
{
	if (PlayerState && BaseLayoutPage)
	{
		UCommonActivatableWidget* const ActivePage = BaseLayoutPage->GetActiveWidget(EPageLayoutStackType::GameUI);
		if (IGameDataModelDrivenInterface* const Interface = Cast<IGameDataModelDrivenInterface>(ActivePage))
		{
			if (UPlayerScoreDataModel* const PlayerScoreDataModel = NewObject<UPlayerScoreDataModel>(ActivePage))
			{
				PlayerScoreDataModel->CurrentScore = FMath::RoundToInt(PlayerState->GetScore());
				Interface->SetDataModel(PlayerScoreDataModel);
			}
		}
	}
}
