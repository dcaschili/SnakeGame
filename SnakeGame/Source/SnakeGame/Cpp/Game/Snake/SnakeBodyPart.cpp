#include "Game/Snake/SnakeBodyPart.h"

#include "Components/StaticMeshComponent.h"
#include "Game/Snake/SnakePawn.h"
#include "Game/Map/MapFunctionLibrary.h"
#include "Data/GameConstants.h"
#include "Game/Components/SnakeBodyPartMoveComponent.h"
#include "Game/Map/MapOccupancyComponent.h"

ASnakeBodyPart::ASnakeBodyPart()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComp;
	StaticMeshComp->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	StaticMeshComp->CastShadow = false;
	StaticMeshComp->SetGenerateOverlapEvents(true);

	SnakeMovementComponent = CreateDefaultSubobject<USnakeBodyPartMoveComponent>(TEXT("SnakeMovementComponent"));

	MapOccupancyComponent = CreateDefaultSubobject<UMapOccupancyComponent>(TEXT("MapOccupancyComponent"));
	if (ensure(MapOccupancyComponent))
	{
		MapOccupancyComponent->SetEnableContinuousTileOccupancyTest(true);
	}
}

void ASnakeBodyPart::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!SnakePawnPtr.IsValid()) return;

	if (!SnakeMovementComponent) return;

	if (!ChangeDirectionQueue.IsEmpty())
	{
		const FChangeDirectionAction ChangeDirAction = ChangeDirectionQueue[0];
		const FVector CurrentPosition = GetActorLocation();

		// Check if we are within the change direction tile.
		if ((CurrentPosition - ChangeDirAction.Location).SquaredLength() <= (HalfTileSize * HalfTileSize))
		{
			// Check if we need to change direction
			if (UMapFunctionLibrary::IsWorldLocationNearCurrentTileCenter(this, CurrentPosition))
			{
				ChangeDirectionQueue.RemoveAt(0);
				SnakeMovementComponent->ChangeMoveDirection(ChangeDirAction.Direction);
			}
		}
	}
}

void ASnakeBodyPart::SetMoveDir(const FVector& InMoveDirection)
{
	if (ensure(SnakeMovementComponent))
	{
		SnakeMovementComponent->ChangeMoveDirection(InMoveDirection);
	}
}

void ASnakeBodyPart::SetSnakePawn(ASnakePawn* InPawnPtr)
{
	if (InPawnPtr)
	{
		SnakePawnPtr = InPawnPtr;
	}
}

ASnakePawn* ASnakeBodyPart::GetSnakePawn() const
{
	if (ensure(SnakePawnPtr.IsValid()))
	{
		return SnakePawnPtr.Get();
	}
	return nullptr;
}

void ASnakeBodyPart::AddChangeDirAction(const FChangeDirectionAction& InChangeDirAction)
{
	ChangeDirectionQueue.Add(InChangeDirAction);
}

FVector ASnakeBodyPart::GetMoveDirection() const
{
	if (ensure(SnakeMovementComponent))
	{
		return SnakeMovementComponent->GetMoveDirection();
	}
	return FVector::RightVector;
}

void ASnakeBodyPart::BeginPlay()
{
	Super::BeginPlay();

	const UGameConstants* const GameConstants = UGameConstants::GetGameConstants(this);
	check(GameConstants);

	HalfTileSize = GameConstants->TileSize / 2.0f;
}

void ASnakeBodyPart::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


