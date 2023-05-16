#include "SnakeGamePlayerState.h"

#include "Kismet/GameplayStatics.h"
#include "SnakeGameGameModeBase.h"
#include "Game/CollectiblesSpawner.h"
#include "SnakeLog.h"
#include "Data/GameConstants.h"

void ASnakeGamePlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	OnPlayerStateScoreUpdated.Broadcast();
}

void ASnakeGamePlayerState::BeginPlay()
{
	Super::BeginPlay();

	// Get collectible spawner actor
	if (ASnakeGameGameModeBase* const SnakeGameMode = Cast<ASnakeGameGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		// On the server
		if (ACollectiblesSpawner* const CollectibleSpawner = SnakeGameMode->GetCollectiblesSpawner())
		{
			CollectibleSpawner->OnCollectibleCollected.AddUniqueDynamic(this, &ThisClass::HandleCollectibleCollected);
		}
	}
}

void ASnakeGamePlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Get collectible spawner actor
	if (ASnakeGameGameModeBase* const SnakeGameMode = Cast<ASnakeGameGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		// On the server
		if (ACollectiblesSpawner* const CollectibleSpawner = SnakeGameMode->GetCollectiblesSpawner())
		{
			CollectibleSpawner->OnCollectibleCollected.RemoveDynamic(this, &ThisClass::HandleCollectibleCollected);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ASnakeGamePlayerState::HandleCollectibleCollected(const FVector& InCollectibleLocation)
{
	GDTUI_SHORT_LOG(SnakeLogCategoryScore, Log, TEXT("Score updated!"));

	const UGameConstants* const GameConstants = UGameConstants::GetGameConstants(this);

	SetScore(GetScore() + GameConstants->BaseCollectibleScore);
	OnPlayerStateScoreUpdated.Broadcast();
}
