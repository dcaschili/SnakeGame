#include "Game/Snake/SnakeBodyPart.h"

#include "Game/Snake/SnakePawn.h"
#include "Game/Map/MapFunctionLibrary.h"
#include "Game/Components/SnakeMoveComponent.h"
#include "Game/Map/MapOccupancyComponent.h"
#include "SnakeLog.h"
#include "Components/BoxComponent.h"
#include "Game/Components/SnakeMoveComponent.h"
#include "Game/ChangeDirectionAction.h"

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
#if !UE_BUILD_SHIPPING
	PrimaryActorTick.bCanEverTick = true;
#else
	PrimaryActorTick.bCanEverTick = false;
#endif // !UE_BUILD_SHIPPING

	SnakeBodyPartCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("SnakeBodyPartCollider"));
	RootComponent = SnakeBodyPartCollider;
	SnakeBodyPartCollider->SetGenerateOverlapEvents(true);
	SnakeBodyPartCollider->SetBoxExtent(FVector{ SnakeBodyBoxColliderExtent });
	SnakeBodyPartCollider->SetLineThickness(SnakeBodyBoxColliderThickness);
	
	BodyPartMoveComp = CreateDefaultSubobject<USnakeMoveComponent>(TEXT("BodyPartMoveComp"));

	MapOccupancyComponent = CreateDefaultSubobject<UMapOccupancyComponent>(TEXT("MapOccupancyComponent"));
	if (ensure(MapOccupancyComponent))
	{
		MapOccupancyComponent->SetEnableContinuousTileOccupancyTest(true);
	}
}

#if !UE_BUILD_SHIPPING
void ASnakeBodyPart::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

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
}
#endif // !UE_BUILD_SHIPPING

void ASnakeBodyPart::SetMoveDir(const FVector& InMoveDirection)
{
	if (ensure(BodyPartMoveComp))
	{
		BodyPartMoveComp->SetMoveDir(InMoveDirection);
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

FVector ASnakeBodyPart::GetMoveDirection() const
{
	if (ensure(BodyPartMoveComp))
	{
		return BodyPartMoveComp->GetMoveDirection();
	}
	return FVector{};
}

void ASnakeBodyPart::BeginPlay()
{
	Super::BeginPlay();
}

void ASnakeBodyPart::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASnakeBodyPart::HandleChangeDirectionAction(const FChangeDirectionAction& NewDirectionAction)
{
	if (BodyPartMoveComp)
	{
		BodyPartMoveComp->AddChangeDirAction(NewDirectionAction);
	}
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