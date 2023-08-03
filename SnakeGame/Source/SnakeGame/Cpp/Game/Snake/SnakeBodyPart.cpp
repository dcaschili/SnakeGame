#include "Game/Snake/SnakeBodyPart.h"

#include "Game/Snake/SnakePawn.h"
#include "Game/Map/MapFunctionLibrary.h"
#include "Data/GameConstants.h"
#include "Game/Components/SnakeMoveComponent.h"
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
	SnakeBodyPartCollider->SetBoxExtent(FVector{ SnakeBodyBoxColliderExtent });
	SnakeBodyPartCollider->SetLineThickness(SnakeBodyBoxColliderThickness);
	
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

	const UGameConstants* const GameConstants = UGameConstants::GetGameConstants(this);
	if (!GameConstants)
	{
		ensure(false);
		return;
	}

	const FVector CurrentPos = GetActorLocation();
	float FrameMovement = GameConstants->MaxMovementSpeed * DeltaSeconds;
	FVector ResultingPos = CurrentPos + MoveDirection * FrameMovement;

	while (!ChangeDirectionQueue.IsEmpty())
	{
		const FChangeDirectionAction ChangeDirAction = ChangeDirectionQueue[0];

		/* 
			If with current frame movement distance we aren't able to reach the tile center 
			defined as a change direction tile, we simply apply the calculated position.
		*/
		// Check if the result pos "overshoots" the change direction location

		/*
			Consider C the point where we have to change direction.
			And assuming that the snake is moving only in the 4 basic directions.
			If the vector that goes from C to the ResultingPos is aligned with
			our current move direction (dot > 0), it means that we are in the "overshoot" case
			we have exceeded the limit and we need to change direction.
			On the contrary, if move direction and the vector are on the opposite direction
			(dot < 0) it means that we didn't reach the point and we can simply apply the 
			motion.
		*/
		const FVector ToTileVector = ResultingPos - ChangeDirAction.Location;
		const bool bOvershoot = MoveDirection.Dot(ToTileVector) >= 0.0;
		if (!bOvershoot)
		{
			// Movement won't reach the change direction tile center, just move the actor.
			break;
		}

		// Move on the tile center reducing the remaining frame movement by that amount.
		FrameMovement = ToTileVector.Length();
		MoveDirection = ChangeDirAction.Direction;
		ResultingPos = ChangeDirAction.Location + MoveDirection * FrameMovement;
		ChangeDirectionQueue.RemoveAt(0);
	}

	SetActorLocation(ResultingPos, true);

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
	MoveDirection = InMoveDirection;
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
	return MoveDirection;	
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