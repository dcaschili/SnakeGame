#pragma once

#include "SnakeGameGameModeBase.h"
#include "SnakeMatchGameModeBase.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndGameDelegate);

UCLASS()
class SNAKEGAME_API ASnakeMatchGameModeBase : public ASnakeGameGameModeBase
{
	GENERATED_BODY()
	
public:

	void EndGame();

	FOnEndGameDelegate OnEndGame{};

};
