#pragma once

#include "SnakeGameGameModeBase.h"
#include "SnakeMatchGameModeBase.generated.h"

class ASnakePawn;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMatchEventDelegate);

UCLASS()
class SNAKEGAME_API ASnakeMatchGameModeBase : public ASnakeGameGameModeBase
{
	GENERATED_BODY()
	
public:
	void StartMatch();
	void EndMatch();

	FOnMatchEventDelegate OnEndMatch{};

	/** Called after player input in the first page of the match state. */
	FOnMatchEventDelegate OnStartMatch{};
	/** Called after the start match delegate. Can be used to perform other initializations. */
	FOnMatchEventDelegate OnMatchStarted{};

private:
	bool bMatchStarted = false;
};
