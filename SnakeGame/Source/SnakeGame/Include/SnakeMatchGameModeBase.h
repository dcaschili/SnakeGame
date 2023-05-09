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
	FOnMatchEventDelegate OnStartMatch{};

private:
	
};
