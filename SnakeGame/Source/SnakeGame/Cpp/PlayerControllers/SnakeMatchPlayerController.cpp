#include "PlayerControllers/SnakeMatchPlayerController.h"

#include "Pages/GDTUIUWBasePage.h"
#include "SnakeLog.h"
#include "CommonActivatableWidget.h"
#include "Pages/GDTUIUWBasePageLayout.h"
#include "SnakeGamePlayerState.h"
#include "Data/Model/PlayerScoreDataModel.h"
#include "Data/Model/GameDataModelDrivenInterface.h"
#include "UI/Pages/GameOverPage.h"

#if !UE_BUILD_SHIPPING
#include "Engine.h"
#endif // !UE_BUILD_SHIPPING

void ASnakeMatchPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ASnakeGamePlayerState* const SnakeGamePlayerState = Cast<ASnakeGamePlayerState>(PlayerState))
	{
		SnakeGamePlayerState->OnPlayerStateScoreUpdated.RemoveDynamic(this, &ThisClass::HandleScoreChanged);
	}

	Super::EndPlay(EndPlayReason);
}

void ASnakeMatchPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputModeGameOnly{};
	SetInputMode(InputModeGameOnly);

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
}

void ASnakeMatchPlayerController::HandleEndGame()
{
	if (GameOverPageClass)
	{
		if (ensure(BaseLayoutPage))
		{
			UGameOverPage* const GameOverPage = Cast<UGameOverPage>(BaseLayoutPage->PushWidget(GameOverPageClass, EPageLayoutStackType::GameUI));
			if (GameOverPage)
			{
				// Record to event

				// Setup model.
			}
		}
	}
	else
	{
		UE_LOG(SnakeLogCategoryUI, Warning, TEXT("ASnakeMatchPlayerController - Missing game over class!"));
		ensure(false);
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
