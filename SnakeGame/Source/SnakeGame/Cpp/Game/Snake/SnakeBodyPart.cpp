#include "Game/Snake/SnakeBodyPart.h"

#include "Game/Snake/SnakePawn.h"
#include "Game/Map/MapFunctionLibrary.h"
#include "Data/GameConstants.h"
#include "Game/Components/SnakeBodyPartMoveComponent.h"
#include "Game/Map/MapOccupancyComponent.h"
#include "SnakeLog.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"

#if !UE_BUILD_SHIPPING
static TAutoConsoleVariable<bool> CVarShowSnakeBodyColliders(
	TEXT("Snake.ShowSnakeBodyColliders"),
	false,
	TEXT("Enables visibility for box collision component on body snake parts.\n")
	TEXT("true: show box collision components. \n")
	TEXT("false: hide box collision components (default) \n"),
	ECVF_Cheat);
#endif // !UE_BUILD_SHIPPING

ASnakeBodyPart::ASnakeBodyPart()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;

	SnakeBodyPartCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("SnakeBodyPartCollider"));
	RootComponent = SnakeBodyPartCollider;
	SnakeBodyPartCollider->SetGenerateOverlapEvents(true);
	SnakeBodyPartCollider->SetBoxExtent(FVector{ 45.0f, 45.0f, 45.0f });
	SnakeBodyPartCollider->SetLineThickness(10.0f);
	
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

	if (!SnakePawnPtr) return;

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

#if !UE_BUILD_SHIPPING
	if (SnakeBodyPartCollider)
	{
		const bool bShowColliders = CVarShowSnakeBodyColliders.GetValueOnGameThread();
		if (bShowColliders && SnakeBodyPartCollider->bHiddenInGame)
		{
			SnakeBodyPartCollider->SetHiddenInGame(false);
		}
		else if(!bShowColliders && !SnakeBodyPartCollider->bHiddenInGame)
		{
			SnakeBodyPartCollider->SetHiddenInGame(true);
		}
	}
#endif // !UE_BUILD_SHIPPING
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
		if (SnakePawnPtr)
		{
			UnbindPawnDelegates();
		}

		SnakePawnPtr = InPawnPtr;
		BindPawnDelegates();
	}
}

ASnakePawn* ASnakeBodyPart::GetSnakePawn() const
{
	return SnakePawnPtr;
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

void ASnakeBodyPart::HandleChangeDirectionAction(const FChangeDirectionAction& NewDirectionAction)
{
	AddChangeDirAction(NewDirectionAction);
}

void ASnakeBodyPart::BindPawnDelegates()
{
	if (SnakePawnPtr)
	{
		SnakePawnPtr->OnChangeDirection.AddUniqueDynamic(this, &ThisClass::HandleChangeDirectionAction);
	}
}

void ASnakeBodyPart::UnbindPawnDelegates()
{
	if (SnakePawnPtr)
	{
		SnakePawnPtr->OnChangeDirection.RemoveDynamic(this, &ThisClass::HandleChangeDirectionAction);
	}
}

void ASnakeBodyPart::SetSnakeBodyPartType(ESnakeBodyPartType InBodyPartType)
{
	BodyPartType = InBodyPartType;
}