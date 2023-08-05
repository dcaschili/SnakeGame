#include "Game/Snake/SnakeBodySplineManager.h"

#include "Components/SplineComponent.h"
#include "Game/Snake/SnakePawn.h"
#include "Game/Snake/SnakeBodyPart.h"
#include "SnakeLog.h"

ASnakeBodySplineManager::ASnakeBodySplineManager()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SnakeBodySplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SnakeBodySplineComponent"));
	RootComponent = SnakeBodySplineComponent;
	SnakeBodySplineComponent->SetMobility(EComponentMobility::Movable);
}

void ASnakeBodySplineManager::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ClearSplinePoints();
}

void ASnakeBodySplineManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);	

	if (!SnakePawn) return;
	if (!SnakeBodySplineComponent) return;

	/*
		ALIGN SPLINE POINTS COUNT TO SNAKE BODY PARTS COUNT + HEAD (if needed)
	*/
	int32 SplinePointCount = SnakeBodySplineComponent->GetNumberOfSplinePoints();
	const int32 SnakeBodyPartsCount = SnakePawn->GetSnakeBodyPartsCount();

	if (SplinePointCount == 0)
	{
		// Add head
		AddSplinePointWorldSpace(SnakePawn->GetActorLocation());
		SplinePointCount = SnakeBodySplineComponent->GetNumberOfSplinePoints();
		GDTUI_SHORT_LOG(SnakeLogCategorySnakeBody, Log, TEXT("Added snake head spline point"));
	}

	int32 BodyPartAsSplinePointsCount = SplinePointCount - 1; // Do not count the head

	if (BodyPartAsSplinePointsCount < SnakeBodyPartsCount)
	{
		GDTUI_SHORT_LOG(SnakeLogCategorySnakeBody, Log, TEXT("Adding spline points for missing body parts."));
		// Missing some body part spline points
		for (int32 i = BodyPartAsSplinePointsCount; i < SnakeBodyPartsCount; ++i)
		{
			if (const ASnakeBodyPart* const SnakeBodyPart = SnakePawn->GetSnakeBodyPartAtIndex(i))
			{
				AddSplinePointWorldSpace(SnakeBodyPart->GetActorLocation());
			}
			else
			{
				GDTUI_LOG(SnakeLogCategorySnakeBody, Error, TEXT("Can't find body part at index: %d. Unable to instance a spline point for that body part."), i);
				ensure(false);
				return;
			}
		}

		SplinePointCount = SnakeBodySplineComponent->GetNumberOfSplinePoints();
		BodyPartAsSplinePointsCount = SplinePointCount - 1;
	}


	/* 
		UPDATE SPLINE POINTS POSITIONS 
	*/
	if (SplinePointCount > 0)
	{
		// Update head
		UpdateSplinePointPositionWorldSpace(0, SnakePawn->GetActorLocation());
	}

	// Update body parts
	if (SnakeBodyPartsCount > 0 && ensure(SnakeBodyPartsCount == BodyPartAsSplinePointsCount))
	{
		for (int32 i = 0; i < SnakeBodyPartsCount; ++i)
		{
			if (const ASnakeBodyPart* const SnakeBodyPart = SnakePawn->GetSnakeBodyPartAtIndex(i))
			{
				UpdateSplinePointPositionWorldSpace(i + 1, SnakeBodyPart->GetActorLocation());
			}
			else
			{
				GDTUI_LOG(SnakeLogCategorySnakeBody, Error, TEXT("Can't find body part at index: %d. Can't update corresponding spline point!"), i);
				ensure(false);
			}
		}
	}


	/*
		UPDATE SPLINE MESH COMPONENTS
	*/
	//if (SnakeBodySplineComponent)
	if (false)
	{
		for (int32 i = 0; i < SnakeBodySplineMeshComponentsList.Num(); ++i)
		{
			USplineMeshComponent* const SplineMeshComp = SnakeBodySplineMeshComponentsList[i];
			if (!SplineMeshComp)
			{
				GDTUI_LOG(SnakeLogCategorySnakeBody, Error, TEXT("Can't find spline mesh component at index: %d."), i);
				ensure(false);
				continue;
			}
		
			const int32 Start = i;
			const int32 End = i + 1;
			ensure(End < SplinePointCount);

			static const bool bUseSplineComp = false;
			FVector StartPos{};
			FVector EndPos{};
			FVector StartTan{};
			FVector EndTan{};

			if (bUseSplineComp)
			{
				/*
					The spline has the concept of direction. The tangets represent the velocity at a spline point
					and therefore they have an orientation that we must keep in mind.
					As I build the points from the head to the body, the direction should be that. For that
					reason I need to set the start point as the front spline point and the end as the position
					of the current body part.
					I don't have a body part that starts from the head, therefore I need that ordering.
					If i want to do the opposite, I would need to negate the tangents.
				*/
				StartPos = SnakeBodySplineComponent->GetLocationAtSplinePoint(Start, ESplineCoordinateSpace::Local);
				EndPos = SnakeBodySplineComponent->GetLocationAtSplinePoint(End, ESplineCoordinateSpace::Local);
				StartTan = SnakeBodySplineComponent->GetTangentAtSplinePoint(Start, ESplineCoordinateSpace::Local);
				EndTan = SnakeBodySplineComponent->GetTangentAtSplinePoint(End, ESplineCoordinateSpace::Local);
			}
			else
			{
				const FTransform& LocalTransform = GetTransform();
				if (Start == 0)
				{
					// Use head
					StartPos = LocalTransform.InverseTransformPosition(SnakePawn->GetActorLocation());
					StartTan = LocalTransform.InverseTransformVector(SnakePawn->GetMoveDirection());

					if (const ASnakeBodyPart* const SnakeBodyPart = SnakePawn->GetSnakeBodyPartAtIndex(End - 1))
					{
						EndPos = LocalTransform.InverseTransformPosition(SnakeBodyPart->GetActorLocation());
						EndTan = LocalTransform.InverseTransformVector(-SnakeBodyPart->GetMoveDirection());
					}
				}
				else
				{
					if (const ASnakeBodyPart* const SnakeBodyPart = SnakePawn->GetSnakeBodyPartAtIndex(Start - 1))
					{
						StartPos = LocalTransform.InverseTransformPosition(SnakeBodyPart->GetActorLocation());
						StartTan = LocalTransform.InverseTransformVector(-SnakeBodyPart->GetMoveDirection());
					}

					if (const ASnakeBodyPart* const SnakeBodyPart = SnakePawn->GetSnakeBodyPartAtIndex(End - 1))
					{
						EndPos = LocalTransform.InverseTransformPosition(SnakeBodyPart->GetActorLocation());
						EndTan = LocalTransform.InverseTransformVector(-SnakeBodyPart->GetMoveDirection());
					}
				}
			}

			SplineMeshComp->SetStartAndEnd(StartPos, StartTan, EndPos, EndTan);
		}
	}
}

void ASnakeBodySplineManager::SetSnakePawn(ASnakePawn* InSnakePawn)
{
	SnakePawn = InSnakePawn;
	if (!SnakePawn) return;	

	ClearSplinePoints();

	// Add snake head
	AddSplinePointWorldSpace(SnakePawn->GetActorLocation());
}

void ASnakeBodySplineManager::ClearSplinePoints()
{
	if (ensure(SnakeBodySplineComponent))
	{
		SnakeBodySplineComponent->ClearSplinePoints();
	}
}

void ASnakeBodySplineManager::AddSplinePointWorldSpace(const FVector& InLocationW)
{
	if (ensure(SnakeBodySplineComponent))
	{
		SnakeBodySplineComponent->AddSplinePoint(InLocationW, ESplineCoordinateSpace::World);

		/*
			CHECK IF A NEW SPLINE MESH COMPONENT IS NEEDED
		*/
		/*const int32 NumOfSplinePoints = SnakeBodySplineComponent->GetNumberOfSplinePoints();
		if (NumOfSplinePoints > 1)
		{
			USplineMeshComponent* const SplineMeshComponent = Cast<USplineMeshComponent>(AddComponentByClass(USplineMeshComponent::StaticClass(), true, FTransform::Identity, false));
			if (SplineMeshComponent)
			{
				GDTUI_SHORT_LOG(SnakeLogCategorySnakeBody, Log, TEXT("Added spline mesh component!"));
				SplineMeshComponent->SetMobility(EComponentMobility::Movable);
				SplineMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

				SplineMeshComponent->SetForwardAxis(SplineMeshAxis);
				SplineMeshComponent->CastShadow = false;
				SplineMeshComponent->SetGenerateOverlapEvents(false);
				if (SplineStaticMesh)
				{
					SplineMeshComponent->SetStaticMesh(SplineStaticMesh);
				}
				else
				{
					GDTUI_PRINT_TO_SCREEN_ERROR(TEXT("Spline manager, missing spline mesh component static mesh."));
					ensure(false);
				}
				SnakeBodySplineMeshComponentsList.Add(SplineMeshComponent);
			}
		}*/
	}
}

void ASnakeBodySplineManager::UpdateSplinePointPositionWorldSpace(int32 InIndex, const FVector& InLocationW)
{
	if (InIndex < 0) return;
	if (!SnakeBodySplineComponent) return;

	SnakeBodySplineComponent->SetLocationAtSplinePoint(InIndex, InLocationW, ESplineCoordinateSpace::World);
}

