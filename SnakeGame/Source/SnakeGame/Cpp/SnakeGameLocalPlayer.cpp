#include "SnakeGameLocalPlayer.h"

#include "SnakeLog.h"

UPlayerProfileSaveGame* USnakeGameLocalPlayer::GetPlayerProfileSaveGame() const
{
	if (bNeedsSaveGameLoad)
	{
		return PlayerProfileSaveGame;
	}
	else
	{
		UE_LOG(SnakeLogCategorySave, Warning, TEXT("USnakeGameLocalPlayer - Player profile savegame missing, you should load the savegame first!"));
		ensure(false);
	}
	return nullptr;
}

void USnakeGameLocalPlayer::SetPlayerProfileSaveGame(UPlayerProfileSaveGame* InPlayerProfileSaveGame)
{
	if (InPlayerProfileSaveGame)
	{
		PlayerProfileSaveGame = InPlayerProfileSaveGame;
		UE_LOG(SnakeLogCategorySave, Log, TEXT("USnakeGameLocalPlayer - Loading player profile save game completed!"));
		bNeedsSaveGameLoad = false;
	}
}
