#include "Game/Components/SnakeBodyPartMoveComponent.h"

#include "Data/GameConstants.h"
#include "SnakeLog.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "SnakeMatchGameModeBase.h"

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

	const UGameConstants* const GameConstants = UGameConstants::GetGameConstants(this);
	check(GameConstants);

	TileSize = GameConstants->TileSize;
	HalfTileSize = TileSize / 2.0f;

	if (bUpdateControlRotation)
	{
		if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
		{
			// Get controller weak reference.
			SnakeController = PawnOwner->GetController();
			if (!SnakeController.IsValid())
			{
				UE_LOG(SnakeLogCategoryGame, Error, TEXT("Can't change rotation without a controller!"));
				ensure(false);
			}
		}
		else
		{
			UE_LOG(SnakeLogCategoryGame, Error, TEXT("Can't change rotation without a controller! Pawn is needed!"));
			check(false);
		}
	}

	if (ASnakeMatchGameModeBase* const SnakeGameMode = Cast<ASnakeMatchGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		// Works only on the server
		SnakeGameMode->OnEndGame.AddUniqueDynamic(this, &ThisClass::HandleEndGame);
	}
	
}

void USnakeBodyPartMoveComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ASnakeMatchGameModeBase* const SnakeGameMode = Cast<ASnakeMatchGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		// Works only on the server
		SnakeGameMode->OnEndGame.RemoveDynamic(this, &ThisClass::HandleEndGame);
	}

	Super::EndPlay(EndPlayReason);
}

void USnakeBodyPartMoveComponent::ChangeMoveDirection(const FVector& InNewDirection)
{
	PreviousMoveDirection = MoveDirection;
	MoveDirection = InNewDirection;
	bDirectionChanged = true;
}

void USnakeBodyPartMoveComponent::HandleEndGame()
{
	bIsMovementEnabled = false;
	UE_LOG(SnakeLogCategoryGame, Verbose, TEXT("USnakeBodyPartMoveComponent - Stop movement!"));
}

void USnakeBodyPartMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!bIsMovementEnabled) return;

	check(GetOwner());
	FVector CurrentPos = GetOwner()->GetActorLocation();

	if (bDirectionChanged)
	{
		if (FMath::IsNearlyZero(MoveDirection.X))
		{
			int32 TmpX = FMath::Abs(CurrentPos.X);
			int32 SignX = FMath::Sign(CurrentPos.X);

			// Center on the vertical coordinate
			/*
				Take the current tile top left coordinate to avoid "jump to next tile" effect.
				If the coordinate is > 0.5, the rounding will move to the next tile.
			*/
			int32 XValue = FMath::Floor(TmpX);
			int32 CurrentTileXValue = XValue - (XValue % TileSize) + HalfTileSize;
			CurrentPos.X = CurrentTileXValue * SignX;
		}
		else if (FMath::IsNearlyZero(MoveDirection.Y))
		{
			int32 TmpY = FMath::Abs(CurrentPos.Y);
			int32 SignY = FMath::Sign(CurrentPos.Y);

			// Center on the horizontal coordinate
			int32 YValue = FMath::Floor(TmpY);
			// Take the current tile top left coordinate.
			int32 CurrentTileYValue = YValue - (YValue % TileSize) + HalfTileSize;
			CurrentPos.Y = CurrentTileYValue * SignY;
		}
		else
		{
			// Something wrong in the movement direction setup
			UE_LOG(SnakeLogCategoryGame, Warning, TEXT("Something went wrong in the MovementDirection setup: %s"), *MoveDirection.ToString());
			check(false);
		}
	}

	const UGameConstants* const GameConstants = UGameConstants::GetGameConstants(this);
	ensure(GameConstants);
	const float MaxMovementSpeed = GameConstants ? GameConstants->MaxMovementSpeed : 1.0f;

	const FVector NewPos = CurrentPos + (MoveDirection * DeltaTime * MaxMovementSpeed);

	GetOwner()->SetActorLocation(NewPos, true);

	if (bUpdateControlRotation && SnakeController.IsValid())
	{
		FRotator FacingDir = UKismetMathLibrary::FindLookAtRotation(NewPos, NewPos + MoveDirection * 500.0f);
		SnakeController->SetControlRotation(FacingDir);
	}
}

void USnakeBodyPartMoveComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USnakeBodyPartMoveComponent, bIsMovementEnabled);
}