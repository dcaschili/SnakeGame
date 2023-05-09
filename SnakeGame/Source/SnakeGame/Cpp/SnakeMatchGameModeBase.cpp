#include "SnakeMatchGameModeBase.h"

#include "SnakeLog.h"
#include "Game/SnakePawn.h"

#if !UE_BUILD_SHIPPING
#include "Engine.h"
#endif //!UE_BUILD_SHIPPING

void ASnakeMatchGameModeBase::StartMatch()
{
#if !UE_BUILD_SHIPPING
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Orange, TEXT("Starting match!"));
#endif // !UE_BUILD_SHIPPING
	GDTUI_SHORT_LOG(SnakeLogCategoryGame, Log, TEXT("Starting match!"));
	OnStartMatch.Broadcast();
}

void ASnakeMatchGameModeBase::EndMatch()
{
#if !UE_BUILD_SHIPPING
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Orange, TEXT("Match ended!"));
#endif // !UE_BUILD_SHIPPING
	GDTUI_SHORT_LOG(SnakeLogCategoryGame, Log, TEXT("Match ended!"));
	OnEndMatch.Broadcast();
}
