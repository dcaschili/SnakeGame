#include "Game/Snake/SnakeBodyPart.h"

#include "Game/Snake/SnakePawn.h"
#include "Game/Map/MapFunctionLibrary.h"
#include "Data/GameConstants.h"
#include "Game/Components/SnakeBodyPartMoveComponent.h"
#include "Game/Map/MapOccupancyComponent.h"
#include "SnakeLog.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"


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

	UpdateSplineMeshComponent();
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

	const USplineComponent* const SplineComp = SnakePawnPtr->GetSplineComponent();
	if (SplineComp)
	{
		const FVector CurrentPos = SplineComp->GetLocationAtSplinePoint(BodyPartIndex.GetValue() + 1, ESplineCoordinateSpace::World);
		const FVector FrontPos = SplineComp->GetLocationAtSplinePoint(BodyPartIndex.GetValue(), ESplineCoordinateSpace::World);
		const FVector CurrentTan = SplineComp->GetTangentAtSplinePoint(BodyPartIndex.GetValue() + 1, ESplineCoordinateSpace::World);
		const FVector FrontTan = SplineComp->GetTangentAtSplinePoint(BodyPartIndex.GetValue(), ESplineCoordinateSpace::World);
		
		SplineMeshComp->SetStartAndEnd(CurrentPos, CurrentTan, FrontPos, FrontTan);
	}
	else
	{
		GDTUI_LOG(SnakeLogCategorySnakeBody, Warning, TEXT("Can't find spline comp in snake pawn. Can't update spline mesh component!"));
	}
}

void ASnakeBodyPart::SetSnakeBodyPartType(ESnakeBodyPartType InBodyPartType)
{
	BodyPartType = InBodyPartType;
}