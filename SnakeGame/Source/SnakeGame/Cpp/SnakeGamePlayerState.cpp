#include "SnakeGamePlayerState.h"

#include "Kismet/GameplayStatics.h"
#include "SnakeGameGameModeBase.h"
#include "Game/CollectiblesSpawner.h"
#include "SnakeLog.h"

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
	UE_LOG(SnakeLogCategoryScore, Log, TEXT("Score updated!"));
	SetScore(GetScore() + 1);
	OnPlayerStateScoreUpdated.Broadcast();
}
