#include "SnakeMatchGameModeBase.h"

#include "SnakeLog.h"

#if !UE_BUILD_SHIPPING
#include "Engine.h"
#endif //!UE_BUILD_SHIPPING

void ASnakeMatchGameModeBase::EndGame()
{
#if !UE_BUILD_SHIPPING
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Orange, TEXT("EndGame called!"));
#endif // !UE_BUILD_SHIPPING
	UE_LOG(SnakeLogCategoryGame, Log, TEXT("EndGame called!"));
	OnEndGame.Broadcast();
}
