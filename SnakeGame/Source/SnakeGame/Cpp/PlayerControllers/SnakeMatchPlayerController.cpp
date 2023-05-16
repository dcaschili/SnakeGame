#include "PlayerControllers/SnakeMatchPlayerController.h"

#include "UI/Pages/GameHUDPage.h"
#include "Pages/GDTUIUWBasePage.h"
#include "SnakeLog.h"
#include "CommonActivatableWidget.h"
#include "Pages/GDTUIUWBasePageLayout.h"
#include "SnakeGamePlayerState.h"
#include "Data/Model/PlayerScoreDataModel.h"
#include "Data/Model/GameDataModelUserInterface.h"
#include "UI/Pages/GameOverPage.h"
#include "Net/UnrealNetwork.h"
#include "SnakeMatchGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "InputTriggers.h"
#include "EnhancedInputSubsystems.h"
#include "Game/SnakePawn.h"
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
		SnakeMatchGameMode->OnEndMatch.RemoveDynamic(this, &ThisClass::HandleEndMatchDelegate);
		SnakeMatchGameMode->OnStartMatch.RemoveDynamic(this, &ThisClass::HandleStartMatchDelegate);
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

	// Setup input mapping
	if (InputMappingContext)
	{
		const ULocalPlayer* const LP = GetLocalPlayer();
		UEnhancedInputLocalPlayerSubsystem* const EnhancedInputSubsystem = LP ? LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;
		if (ensure(EnhancedInputSubsystem))
		{
			EnhancedInputSubsystem->AddMappingContext(InputMappingContext, 0);
		}
		else
		{
			GDTUI_LOG(SnakeLogCategoryGame, Error, TEXT("Can't find EnhancedInputSubsystem!"));
			ensure(false);
		}
	}
	else
	{
		GDTUI_LOG(SnakeLogCategoryGame, Error, TEXT("Missing InputMapping Context"));
		ensure(false);
	}

	if (ASnakeGamePlayerState* const SnakeGamePlayerState = Cast<ASnakeGamePlayerState>(PlayerState))
	{
		SnakeGamePlayerState->OnPlayerStateScoreUpdated.AddUniqueDynamic(this, &ThisClass::HandleScoreChanged);
	}

	if (BaseLayoutPage)
	{
		if (HUDMatchPageClass)
		{
			BaseLayoutPage->PushWidget(HUDMatchPageClass, EPageLayoutStackType::GameUI);
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
		SnakeMatchGameMode->OnEndMatch.AddUniqueDynamic(this, &ThisClass::HandleEndMatchDelegate);
		SnakeMatchGameMode->OnStartMatch.AddUniqueDynamic(this, &ThisClass::HandleStartMatchDelegate);
	}
}

void ASnakeMatchPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (ensure(EnhancedInputComponent))
	{
		if (StartMatchIA)
		{
			EnhancedInputComponent->BindAction(StartMatchIA, ETriggerEvent::Triggered, this, &ThisClass::HandleStartMatchAction);
		}
		else
		{
			GDTUI_SHORT_LOG(SnakeLogCategoryGame, Warning, TEXT("Missing Start Match IA"));
			ensure(false);
		}

		if (CloseGameIA)
		{
			EnhancedInputComponent->BindAction(CloseGameIA, ETriggerEvent::Triggered, this, &ThisClass::HandleCloseGameAction);
		}
		else
		{
			GDTUI_SHORT_LOG(SnakeLogCategoryGame, Warning, TEXT("Missing CloseGameIA"));
			ensure(false);
		}
	}
}

void ASnakeMatchPlayerController::Multicast_EndMatch_Implementation()
{
	GDTUI_LOG(SnakeLogCategoryGame, Verbose, TEXT("Received EndGame from server"));
	InnerHandleEndMatch();
}

void ASnakeMatchPlayerController::HandleCloseGameAction(const FInputActionInstance& InputActionInstance)
{
	GDTUI_LOG(SnakeLogCategoryGame, Log, TEXT("Close game requested!"));

	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
}

void ASnakeMatchPlayerController::HandleStartMatchAction(const FInputActionInstance& InputActionInstance)
{
	GDTUI_LOG(SnakeLogCategoryGame, Log, TEXT("Start match requested!"));

	Server_StartMatch();
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
				SnakeLocalPlayer->UpdatePlayerScore(FMath::RoundToInt(PlayerState->GetScore()));
				SnakeLocalPlayer->SaveGame();
			}
		}

		// Go back to main menu
		ClientTravel(FString(TEXT("Game/Maps/Menu")), TRAVEL_Absolute);
	}
}

void ASnakeMatchPlayerController::HandleEndMatchDelegate()
{
	GDTUI_LOG(SnakeLogCategoryGame, Verbose, TEXT("Received endgame delegate event on the server!"));
	
	// Forward event to all client.
	Multicast_EndMatch();
}

void ASnakeMatchPlayerController::HandleStartMatchDelegate()
{
	if (ensure(SnakePawnClass))
	{
		GDTUI_SHORT_LOG(SnakeLogCategoryGame, Log, TEXT("Spawning snake pawn!"));
		
		ASnakeMatchGameModeBase* const SnakeMatchGameMode = Cast<ASnakeMatchGameModeBase>(UGameplayStatics::GetGameMode(this));
		AActor* const PlayerStart = SnakeMatchGameMode ? SnakeMatchGameMode->FindPlayerStart(this) : nullptr;

		if (PlayerStart)
		{
			if (UWorld* World = GetWorld())
			{
				GDTUI_SHORT_LOG(SnakeLogCategoryGame, Log, TEXT("Snake spawn completed!"));
				ASnakePawn* const SnakePawn = World->SpawnActor<ASnakePawn>(SnakePawnClass, PlayerStart->GetActorLocation(), FRotator::ZeroRotator);
				
				Possess(SnakePawn);
			}
		}
	}
	else
	{
		GDTUI_LOG(SnakeLogCategoryGame, Error, TEXT("Missing snake pawn class!"));
		ensure(false);
	}

	// Show HUD page
	if (BaseLayoutPage)
	{
		UGameHUDPage* const GameHUDPage = Cast<UGameHUDPage>(BaseLayoutPage->GetActiveWidget(EPageLayoutStackType::GameUI));
		if (GameHUDPage)
		{
			GameHUDPage->SetHUDConfigurationActive(true);
			UpdatePageScore();
		}
	}
}

void ASnakeMatchPlayerController::InnerHandleEndMatch()
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

					// Setup data model.
					if (UPlayerScoreDataModel* const ScoreDataModel = NewObject<UPlayerScoreDataModel>(GameOverPage))
					{
						const USnakeGameLocalPlayer* const SnakeGameLocalPlayer = Cast<USnakeGameLocalPlayer>(GetLocalPlayer());
						if (SnakeGameLocalPlayer && PlayerState)
						{
							ScoreDataModel->BestScore = SnakeGameLocalPlayer->GetBestScore();
							ScoreDataModel->CurrentScore = FMath::RoundToInt(PlayerState->GetScore());
							ScoreDataModel->bIsBestScore = ScoreDataModel->BestScore < ScoreDataModel->CurrentScore;

							if (ScoreDataModel->bIsBestScore)
							{
								GDTUI_SHORT_LOG(SnakeLogCategoryGame, Log, TEXT("New record!"));
							}
							else
							{
								GDTUI_SHORT_LOG(SnakeLogCategoryGame, Log, TEXT("No new record!"));
							}

							GameOverPage->SetDataModel(ScoreDataModel);
						}
					}
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
		if (IGameDataModelUserInterface* const Interface = Cast<IGameDataModelUserInterface>(ActivePage))
		{
			if (UPlayerScoreDataModel* const PlayerScoreDataModel = NewObject<UPlayerScoreDataModel>(ActivePage))
			{
				PlayerScoreDataModel->CurrentScore = FMath::RoundToInt(PlayerState->GetScore());
				Interface->SetDataModel(PlayerScoreDataModel);
			}
		}
	}
}

void ASnakeMatchPlayerController::Server_StartMatch_Implementation()
{
	ASnakeMatchGameModeBase* const SnakeMatchGameModeBase = Cast<ASnakeMatchGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (ensure(SnakeMatchGameModeBase))
	{
		SnakeMatchGameModeBase->StartMatch();
	}
}

bool ASnakeMatchPlayerController::Server_StartMatch_Validate()
{
	return true;
}