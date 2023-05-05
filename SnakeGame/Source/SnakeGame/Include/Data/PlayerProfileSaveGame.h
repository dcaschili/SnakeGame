#pragma once

#include "GameFramework/SaveGame.h"

#include "PlayerProfileSaveGame.generated.h"

UCLASS()
class SNAKEGAME_API UPlayerProfileSaveGame : public USaveGame
{
    GENERATED_BODY()
public:
	
	static const FString PlayerProfileSlotName;

	UPROPERTY()
	int32 Score = -1;
};