#include "Game/Components/SnakeMoveComponent.h"

#include "Data/GameConstants.h"
#include "GameFramework/Pawn.h"
#include "SnakeLog.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Controller.h"
#include "Game/Map/MapFunctionLibrary.h"

USnakeMoveComponent::USnakeMoveComponent()
	: Super()
{
	SetIsReplicatedByDefault(true);

	bWantsInitializeComponent = false;
	PrimaryComponentTick.bCanEverTick = true;
}

void USnakeMoveComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USnakeMoveComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void USnakeMoveComponent::SetMoveDir(const FVector& InNewDir)
{
	MoveDirection = InNewDir;
}

void USnakeMoveComponent::AddChangeDirAction(const FChangeDirectionAction& InChangeDirection)
{
	ChangeDirectionQueue.Add(InChangeDirection);
}

void USnakeMoveComponent::SetChangeDirActionQueue(const TArray<FChangeDirectionAction>& InChangeDirQueue)
{
	TArray<FChangeDirectionAction> tmp = InChangeDirQueue;
	SetChangeDirActionQueue(MoveTemp(tmp));
}

void USnakeMoveComponent::SetChangeDirActionQueue(TArray<FChangeDirectionAction>&& InChangeDirQueue)
{
	ChangeDirectionQueue = MoveTemp(InChangeDirQueue);
}

AController* USnakeMoveComponent::GetOwningController() const
{
	if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
	{
		return PawnOwner->GetController();
	}
	return nullptr;
}

void USnakeMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT("SnakeMoveComp"));

	if (ensure(GetOwner()))
	{
		const UGameConstants* const GameConstants = UGameConstants::GetGameConstants(this);
		if (!GameConstants)
		{
			ensure(false);
			return;
		}

		const float CurrentZ = GetOwner()->GetActorLocation().Z;
		FVector CurrentPos = GetOwner()->GetActorLocation();
		CurrentPos.Z = 0.f;

		float FrameMovement = GameConstants->MaxMovementSpeed * DeltaTime;
		FVector ResultingPos = CurrentPos + MoveDirection * FrameMovement;

		while (!ChangeDirectionQueue.IsEmpty())
		{
			const FChangeDirectionAction ChangeDirAction = ChangeDirectionQueue[0];
			FVector ChangeDirLocation = ChangeDirAction.Location;
			ChangeDirLocation.Z = 0.f;

			/*
				If with current frame movement distance we aren't able to reach the tile center
				defined as a change direction tile, we simply apply the calculated position.
			*/
			// Check if the result pos "overshoots" the change direction location
					
			const bool bOvershoot = UMapFunctionLibrary::DoesOvershootPosition(this, ResultingPos, MoveDirection, ChangeDirAction.Location);;
			if (!bOvershoot)
			{
				// Movement won't reach the change direction tile center, just move the actor.
				break;
			}

			// Move on the tile center reducing the remaining frame movement by that amount.
			FrameMovement = (ResultingPos - ChangeDirLocation).Length();
			MoveDirection = ChangeDirAction.Direction;
			ResultingPos = ChangeDirLocation + MoveDirection * FrameMovement;
			ChangeDirectionQueue.RemoveAt(0);
		}

		ResultingPos.Z = CurrentZ;
		GetOwner()->SetActorLocation(ResultingPos, true);
		
		if (bUpdateControlRotation)
		{
			if (AController* const Controller = GetOwningController())
			{
				const FRotator FacingDir = UKismetMathLibrary::FindLookAtRotation(ResultingPos, ResultingPos + MoveDirection * 500.0f);
				Controller->SetControlRotation(FacingDir);
			}
			else
			{
				GDTUI_LOG(SnakeLogCategoryGame, Error, TEXT("Unable to retrieve Controller! To use bUpdateControlRotation this component must be used on a pawn with a controller!"));
				ensure(false);
			}
		}
	}

}
