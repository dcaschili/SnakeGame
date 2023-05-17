#include "SnakeMatchGameModeBase.h"

#include "SnakeLog.h"
#include "Game/SnakePawn.h"

#if !UE_BUILD_SHIPPING
#include "Engine.h"
#endif //!UE_BUILD_SHIPPING

void ASnakeMatchGameModeBase::StartMatch()
{
	if (bMatchStarted)
	{
		GDTUI_LOG(SnakeLogCategoryGame, Warning, TEXT("Start match called multiple times!"));
		ensure(false);
		return;
	}


#if !UE_BUILD_SHIPPING
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Orange, TEXT("Starting match!"));
#endif // !UE_BUILD_SHIPPING
	GDTUI_SHORT_LOG(SnakeLogCategoryGame, Log, TEXT("Starting match!"));
	OnStartMatch.Broadcast();

	/*
		This is useful to perform collectible spawning given that it needs
		to have an occupancy map updated with the snake.
		Being after the OnStartMatch delegate, I'm sure that this will happen 
		afterwards.
	*/
	GDTUI_SHORT_LOG(SnakeLogCategoryGame, Log, TEXT("Match started!"));
	OnMatchStarted.Broadcast();

	bMatchStarted = true;
}

void ASnakeMatchGameModeBase::EndMatch()
{
#if !UE_BUILD_SHIPPING
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Orange, TEXT("Match ended!"));
#endif // !UE_BUILD_SHIPPING
	GDTUI_SHORT_LOG(SnakeLogCategoryGame, Log, TEXT("Match ended!"));
	OnEndMatch.Broadcast();

	bMatchStarted = false;
}
