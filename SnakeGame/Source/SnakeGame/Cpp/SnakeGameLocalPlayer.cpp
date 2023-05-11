#include "SnakeGameLocalPlayer.h"

#include "SnakeLog.h"
#include "Data/PlayerProfileSaveGame.h"
#include "Kismet/GameplayStatics.h"

UPlayerProfileSaveGame* USnakeGameLocalPlayer::GetPlayerProfileSaveGame() const
{
	if (!bNeedsSaveGameLoad)
	{
		return PlayerProfileSaveGame;
	}
	else
	{
		GDTUI_LOG(SnakeLogCategorySave, Warning, TEXT("Player profile savegame missing, you should load the savegame first!"));
		ensure(false);
	}
	return nullptr;
}

void USnakeGameLocalPlayer::SetPlayerProfileSaveGame(UPlayerProfileSaveGame* InPlayerProfileSaveGame)
{
	if (InPlayerProfileSaveGame)
	{
		PlayerProfileSaveGame = InPlayerProfileSaveGame;
		GDTUI_SHORT_LOG(SnakeLogCategorySave, Log, TEXT("Loading player profile save game completed!"));
		bNeedsSaveGameLoad = false;
	}
}

void USnakeGameLocalPlayer::UpdatePlayerScore(int32 InNewScore)
{
	if (InNewScore >= 0)
	{
		if (ensure(PlayerProfileSaveGame) && InNewScore > PlayerProfileSaveGame->Score)
		{
			PlayerProfileSaveGame->Score = InNewScore;
		}
	}
}

void USnakeGameLocalPlayer::SaveGame()
{
	if (ensure(PlayerProfileSaveGame) && IsPrimaryPlayer())
	{
		// Save only on main player.
		if (UGameplayStatics::SaveGameToSlot(PlayerProfileSaveGame, UPlayerProfileSaveGame::PlayerProfileSlotName, 0))
		{
			GDTUI_SHORT_LOG(SnakeLogCategorySave, Log, TEXT("Savegame completed!"));
		}
		else
		{
			GDTUI_LOG(SnakeLogCategorySave, Error, TEXT("Unable to save profile!"));			
			ensure(false);
		}
	}
}

int32 USnakeGameLocalPlayer::GetBestScore() const
{
	if (PlayerProfileSaveGame)
	{
		return PlayerProfileSaveGame->Score;
	}
	else
	{
		GDTUI_LOG(SnakeLogCategorySave, Warning, TEXT("Requested best score without a savegame!"));
		ensure(false);
	}
	return -1;
}
