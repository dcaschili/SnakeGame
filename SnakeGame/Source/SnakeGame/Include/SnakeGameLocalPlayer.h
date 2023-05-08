#pragma once

#include "Engine/LocalPlayer.h"

#include "SnakeGameLocalPlayer.generated.h"

class UPlayerProfileSaveGame;

UCLASS(Blueprintable, BlueprintType)
class SNAKEGAME_API USnakeGameLocalPlayer : public ULocalPlayer
{
    GENERATED_BODY()
public:

	bool GetNeedsSaveGameLoad() const { return bNeedsSaveGameLoad; }

	UPlayerProfileSaveGame* GetPlayerProfileSaveGame() const;
	void SetPlayerProfileSaveGame(UPlayerProfileSaveGame* InPlayerProfileSaveGame);

	void UpdatePlayerScore(int32 InNewScore);
	void SaveGame();

private:
	UPROPERTY()
	TObjectPtr<UPlayerProfileSaveGame> PlayerProfileSaveGame{};

	bool bNeedsSaveGameLoad = true;
};