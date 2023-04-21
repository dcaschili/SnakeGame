#include "Game/SnakeBodyPartSpawner.h"

#include "Data/GameConstants.h"

ASnakeBodyPartSpawner::ASnakeBodyPartSpawner()
	: Super()
{
	BoxCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box collision component."));	
	RootComponent = BoxCollisionComponent;

	const UGameConstants* GameConstants = UGameConstants::GetGameConstants(this);
	if (!GameConstants)
	{
		static ConstructorHelpers::FObjectFinder<UGameConstants> DefaultAsset(TEXT("/Game/Data/GameConstants"));
		GameConstants = DefaultAsset.Object;
		check(GameConstants);
	}

	if (GameConstants)
	{
		const float HalfTileSize = GameConstants->TileSize / 2.0f;
		BoxCollisionComponent->SetBoxExtent({ HalfTileSize, HalfTileSize, HalfTileSize });
	}
}

void ASnakeBodyPartSpawner::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (OtherActor)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::Printf(TEXT("End overlap with: %s"), *OtherActor->GetName()));
	}
}

void ASnakeBodyPartSpawner::BeginPlay()
{
	Super::BeginPlay();
}
