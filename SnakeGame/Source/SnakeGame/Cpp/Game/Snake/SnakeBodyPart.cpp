#include "Game/Snake/SnakeBodyPart.h"

#include "Game/Snake/SnakePawn.h"
#include "Game/Map/MapFunctionLibrary.h"
#include "Data/GameConstants.h"
#include "Game/Components/SnakeBodyPartMoveComponent.h"
#include "Game/Map/MapOccupancyComponent.h"
#include "SnakeLog.h"
#include "Components/BoxComponent.h"


ASnakeBodyPart::ASnakeBodyPart()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;

	
	SnakeBodyPartCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("SnakeBodyPartCollider"));
	RootComponent = SnakeBodyPartCollider;
	SnakeBodyPartCollider->SetGenerateOverlapEvents(true);
	SnakeBodyPartCollider->SetBoxExtent(FVector{ 50.0f, 50.0f, 50.0f });

	SplineMeshComp = CreateDefaultSubobject<USplineMeshComponent>(TEXT("SplineMeshComponent"));
	SplineMeshComp->SetupAttachment(RootComponent);
	SplineMeshComp->SetForwardAxis(SplineMeshAxis.GetValue());
	SplineMeshComp->CastShadow = false;
	SplineMeshComp->SetGenerateOverlapEvents(false);
	SplineMeshComp->SetMobility(EComponentMobility::Movable);
	if (SplineStaticMesh)
	{
		SplineMeshComp->SetStaticMesh(SplineStaticMesh);
	}
	
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

	UpdateSplineMeshComponent();

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
}

#if WITH_EDITOR
void ASnakeBodyPart::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName.IsEqual(GET_MEMBER_NAME_CHECKED(ASnakeBodyPart, SplineStaticMesh)))
	{
		if (SplineStaticMesh && SplineMeshComp)
		{
			SplineMeshComp->SetStaticMesh(SplineStaticMesh);
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

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

void ASnakeBodyPart::SetSnakeBodyPartIndex(int32 InBodyPartIndex)
{
	if (InBodyPartIndex >= 0)
	{
		GDTUI_SHORT_LOG(SnakeLogCategorySnakeBody, Verbose, TEXT("Setup body part index: %d"), InBodyPartIndex);
		BodyPartIndex = InBodyPartIndex;
	}
	else
	{
		GDTUI_LOG(SnakeLogCategorySnakeBody, Warning, TEXT("Trying to setup a negative body part index!"));
		ensure(false);
	}
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

void ASnakeBodyPart::UpdateSplineMeshComponent()
{
	if (!BodyPartIndex.IsSet())
	{
		ensure(false);
		return;
	}
	
	if (!SnakePawnPtr)
	{
		ensure(false);
		return;
	}

	if (!SplineMeshComp)
	{
		ensure(false);
		return;
	}

	const FVector CurrentPos = GetTransform().InverseTransformPosition(GetActorLocation());
	const FVector CurrentMovDir = GetMoveDirection();

	FVector FrontPosition{};
	FVector FrontMoveDir{};
	if (BodyPartIndex.GetValue() == 0)
	{
		// Take head position
		FrontPosition = SnakePawnPtr->GetActorLocation();
		FrontMoveDir = SnakePawnPtr->GetMoveDirection();
	}
	else if(BodyPartIndex.GetValue() > 0)
	{
		const int32 FrontBodyPartIndex = BodyPartIndex.GetValue() - 1;
		if (const ASnakeBodyPart* const FrontBodyPart = SnakePawnPtr->GetSnakeBodyPartAtIndex(FrontBodyPartIndex))
		{
			FrontPosition = FrontBodyPart->GetActorLocation();
			FrontMoveDir = FrontBodyPart->GetMoveDirection();
		}
		else
		{
			GDTUI_LOG(SnakeLogCategorySnakeBody, Warning, TEXT("Can't find front body part at index: %d"), BodyPartIndex.GetValue());
			ensure(false);
			return;
		}
	}
	else
	{
		GDTUI_LOG(SnakeLogCategorySnakeBody, Error, TEXT("Negative snake body part index"));
		ensure(false);
		return;
	}
		
	FrontPosition = GetTransform().InverseTransformPosition(FrontPosition);
	SplineMeshComp->SetStartAndEnd(CurrentPos, CurrentMovDir, FrontPosition, FrontMoveDir);
}

void ASnakeBodyPart::SetSnakeBodyPartType(ESnakeBodyPartType InBodyPartType)
{
	BodyPartType = InBodyPartType;
}