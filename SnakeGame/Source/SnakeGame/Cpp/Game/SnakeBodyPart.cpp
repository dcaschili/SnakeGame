#include "Game/SnakeBodyPart.h"

#include "Components/StaticMeshComponent.h"
#include "Game/SnakePawn.h"
#include "Game/Map/MapFunctionLibrary.h"
#include "Data/GameConstants.h"

ASnakeBodyPart::ASnakeBodyPart()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComp;
	StaticMeshComp->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	StaticMeshComp->CastShadow = false;
}

void ASnakeBodyPart::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!SnakePawnPtr.IsValid()) return;

	if (!ChangeDirectionQueue.IsEmpty())
	{
		if (const FChangeDirectionAction* ChangeDirAction = ChangeDirectionQueue.Peek())
		{
			const FVector CurrentPosition = GetActorLocation();

			// Check if we are within the change direction tile.
			if ((CurrentPosition - ChangeDirAction->Location).SquaredLength() <= (HalfTileSize * HalfTileSize))
			{
				// Check if we need to change direction
				if (UMapFunctionLibrary::IsWorldLocationNearCurrentTileCenter(this, CurrentPosition))
				{
					FChangeDirectionAction CurrentChangeDirectionAction{};
					ensure(ChangeDirectionQueue.Dequeue(CurrentChangeDirectionAction));
					
					// Change direction
					CurrentMoveDirection = CurrentChangeDirectionAction.Direction;
				}
			}
		}
	}
	
}

void ASnakeBodyPart::SetSnakePawn(ASnakePawn* InPawnPtr)
{
	if (InPawnPtr)
	{
		SnakePawnPtr = InPawnPtr;

		BindDelegates();
	}
}

void ASnakeBodyPart::SetBodyPartType(ESnakeBodyPartType NewBodyPartType)
{
	BodyPartType = NewBodyPartType;
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
	UnbindDelegates();
	Super::EndPlay(EndPlayReason);
}

void ASnakeBodyPart::HandleChangeDirectionDelegate(const FChangeDirectionAction& InNewChangeDirection)
{
	ChangeDirectionQueue.Enqueue(InNewChangeDirection);
}

void ASnakeBodyPart::BindDelegates()
{
	if (SnakePawnPtr.IsValid())
	{
		SnakePawnPtr->OnChangeDirection.AddUniqueDynamic(this, &ThisClass::HandleChangeDirectionDelegate);
	}
}

void ASnakeBodyPart::UnbindDelegates()
{
	if (SnakePawnPtr.IsValid())
	{
		SnakePawnPtr->OnChangeDirection.RemoveDynamic(this, &ThisClass::HandleChangeDirectionDelegate);
	}
}
