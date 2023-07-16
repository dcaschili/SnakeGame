#include "Game/Components/SnakeBodyPartMoveComponent.h"

#include "Data/GameConstants.h"
#include "GameFramework/Pawn.h"
#include "SnakeLog.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Controller.h"
#include "Game/Map/MapFunctionLibrary.h"

USnakeBodyPartMoveComponent::USnakeBodyPartMoveComponent()
	: Super()
{
	SetIsReplicatedByDefault(true);

	bWantsInitializeComponent = false;
	PrimaryComponentTick.bCanEverTick = true;
}

void USnakeBodyPartMoveComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USnakeBodyPartMoveComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void USnakeBodyPartMoveComponent::ChangeMoveDirection(const FVector& InNewDirection)
{
	PreviousMoveDirection = MoveDirection;
	MoveDirection = InNewDirection;
	bDirectionChanged = true;
}

AController* USnakeBodyPartMoveComponent::GetOwningController() const
{
	if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
	{
		return PawnOwner->GetController();
	}
	return nullptr;
}

void USnakeBodyPartMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	check(GetOwner());
	FVector CurrentPos = GetOwner()->GetActorLocation();	
	
	const UGameConstants* const GameConstants = UGameConstants::GetGameConstants(this);
	ensure(GameConstants);
	float MovementAmountWithDeltaTime = (GameConstants ? GameConstants->MaxMovementSpeed : 1.0f) * DeltaTime;

	if (bDirectionChanged)
	{
		FVector TileCenter{};
		if (UMapFunctionLibrary::AlignWorldLocationToTileCenter(this, CurrentPos, TileCenter))
		{
			FVector TmpCurrentPos = CurrentPos;
			TmpCurrentPos.Z = 0.0f;

			float DistanceToTileCenter = (TileCenter - TmpCurrentPos).Length();
			ensure(MovementAmountWithDeltaTime >= DistanceToTileCenter);

			/*
				Take into consideration the movement executed to 
				reach the tile center.
			*/
			MovementAmountWithDeltaTime -= DistanceToTileCenter;
			
			/* 
				Setup position on tile center waiting to move 
				toward the new direction by the remaining amount
			*/
			TmpCurrentPos = TileCenter;
			TmpCurrentPos.Z = CurrentPos.Z;

			CurrentPos = TmpCurrentPos;			
			
			bDirectionChanged = false;
		}
		else
		{			
			GDTUI_LOG(SnakeLogCategoryGame, Error, TEXT("Unable to find a corresponding map tile for current position!"));
			check(false);
		}
	}

	const FVector NewPos = CurrentPos + (MoveDirection * MovementAmountWithDeltaTime);

	GetOwner()->SetActorLocation(NewPos, true);

	if (bUpdateControlRotation)
	{
		if (AController* const Controller = GetOwningController())
		{
			const FRotator FacingDir = UKismetMathLibrary::FindLookAtRotation(NewPos, NewPos + MoveDirection * 500.0f);
			Controller->SetControlRotation(FacingDir);
		}
		else
		{
			GDTUI_LOG(SnakeLogCategoryGame, Error, TEXT("Unable to retrieve Controller! To use bUpdateControlRotation this component must be used on a pawn with a controller!"));
			ensure(false);
		}
	}
}
