#include "Game/SnakePawn.h"

#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SnakeLog.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "TimerManager.h"
#include "SnakeGameInstance.h"
#include "Data/GameConstants.h"


#if !UE_BUILD_SHIPPING
#include "DrawDebugHelpers.h"
#include "Utils/GDTCDebugFunctionLibrary.h"

static TAutoConsoleVariable<bool> CVarSnakePositionDebug(
	TEXT("Snake.EnableSnakePositionDebug"),
	false,
	TEXT("Enable the snake position debugger:\n")
	TEXT("true: draw a sphere on the previous position.\n")
	TEXT("false: disable debugger.\n"),
	ECVF_Cheat);
#endif // !UE_BUILD_SHIPPING

ASnakePawn::ASnakePawn()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	RootComponent = StaticMeshComp;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	if (SpringArmComp)
	{
		SpringArmComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		SpringArmComp->bInheritPitch = false;
		SpringArmComp->bInheritYaw = false;
		SpringArmComp->bInheritRoll = false;
		SpringArmComp->TargetArmLength = 800.0f;
		// Needed to reduce the abrupt change of direction due to the "snap to tile" movement
		SpringArmComp->bEnableCameraLag = true;
	}

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	if (CameraComp)
	{
		CameraComp->AttachToComponent(SpringArmComp, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void ASnakePawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

#if !UE_BUILD_SHIPPING
	if (CVarSnakePositionDebug.GetValueOnGameThread())
	{
		// Check if a timer is already active.
		if (!SnakePositionDebuggerTimerHandle.IsValid())
		{
			GetWorldTimerManager().SetTimer(SnakePositionDebuggerTimerHandle, [this]() {
				FVector CurrentPos = GetActorLocation();
				CurrentPos.Z = 0.0f;
				DrawDebugSphere(GetWorld(), CurrentPos, 50.0f, 32, FColor::Red, false, 2.0f);
				}, 0.05f, true);
		}
	}
	else
	{
		// Disable draw debug if it is active.
		if (SnakePositionDebuggerTimerHandle.IsValid())
		{
			GetWorldTimerManager().ClearTimer(SnakePositionDebuggerTimerHandle);
		}
	}
#endif // !UE_BUILD_SHIPPING


	const FVector CurrentPos = GetActorLocation();
	FVector NewPos = CurrentPos + (MoveDirection * DeltaSeconds * MaxMovementSpeed);
			
	// Center in tile
	if (bDirectionChanged && PreviousDirection.IsSet())
	{
		bDirectionChanged = false;

		// Avoid sharp angle during direction change.
		const FVector PreviousDir = PreviousDirection.GetValue();
		FVector NewDir = MoveDirection + PreviousDir;
		NewDir.Normalize();
		NewPos = CurrentPos + (NewDir * DeltaSeconds * MaxMovementSpeed);

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
			// TODO: 100 must be a config value, move to game constants data asset.
			int32 CurrentTileXValue = XValue - (XValue % TileSize) + HalfTileSize;
			//NewPos.X = CurrentTileXValue + 50 * FMath::Sign(NewPos.X);
			//NewPos.X = CurrentTileXValue + 50;
			NewPos.X = CurrentTileXValue * SignX;
		}
		else if (FMath::IsNearlyZero(MoveDirection.Y))
		{
			int32 TmpY = FMath::Abs(CurrentPos.Y);
			int32 SignY = FMath::Sign(CurrentPos.Y);

			// Center on the horizontal coordinate
			int32 YValue = FMath::Floor(TmpY);
			// Take the current tile top left coordinate.
			// TODO: 100 must be a config value, move to game constants data asset.
			int32 CurrentTileYValue = YValue - (YValue % TileSize) + HalfTileSize;
			//NewPos.Y = CurrentTileYValue + 50 * FMath::Sign(NewPos.Y);
			NewPos.Y = CurrentTileYValue * SignY;
		}
		else
		{
			// Something wrong in the movement direction setup
			UE_LOG(SnakeLogCategoryGame, Warning, TEXT("Something went wrong in the MovementDirection setup: %s"), *MoveDirection.ToString());
			ensure(false);
		}
	}

	SetActorLocation(NewPos, true);

	if (Controller)
	{
		FRotator FacingDir = UKismetMathLibrary::FindLookAtRotation(NewPos, NewPos + MoveDirection * 500.0f);
		Controller->SetControlRotation(FacingDir);
	}
}

void ASnakePawn::BeginPlay()
{
	Super::BeginPlay();

	// Setup input
	if (InputMappingContext)
	{
		APlayerController* PC = Cast<APlayerController>(Controller);
		ULocalPlayer* LocalPlayer = PC ? PC->GetLocalPlayer() : nullptr;
		if (LocalPlayer)
		{
			UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
			if (EnhancedInputSubsystem)
			{
				EnhancedInputSubsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
	else
	{
		UE_LOG(SnakeLogCategoryGame, Warning, TEXT("Missing InputMapping Context"));
		ensure(false);
	}

	const USnakeGameInstance* const GameInstance = Cast<USnakeGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(GameInstance);
	const UGameConstants* const GameConstants = GameInstance->GetGameConstants();
	check(GameConstants);
	TileSize = GameConstants->TileSize;
	HalfTileSize = FMath::RoundToInt32(TileSize / 2.0f);
}

void ASnakePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (ensure(EnhancedInputComponent))
	{
		if (MoveRightIA)
		{
			EnhancedInputComponent->BindAction(MoveRightIA, ETriggerEvent::Triggered, this, &ThisClass::HandleMoveRightIA);
		}
		else
		{
			UE_LOG(SnakeLogCategoryGame, Warning, TEXT("Missing MoveRightIA!"));
			ensure(false);
		}
		
		if (MoveUpIA)
		{
			EnhancedInputComponent->BindAction(MoveUpIA, ETriggerEvent::Triggered, this, &ThisClass::HandleMoveUpIA);
		}
		else
		{
			UE_LOG(SnakeLogCategoryGame, Warning, TEXT("Missing MoveUpIA!"));
			ensure(false);
		}
	}
}

void ASnakePawn::HandleMoveRightIA(const FInputActionInstance& InputActionInstance)
{
	if (InputActionInstance.GetValue().IsNonZero())
	{
		// Can't change direction left/right without first going up or down.
		if (FMath::IsNearlyZero(MoveDirection.Y))
		{
			const float Amount = InputActionInstance.GetValue().Get<float>();
			PreviousDirection = MoveDirection;
			MoveDirection = FVector(0.0f, Amount, 0.0f);
			bDirectionChanged = true;
		}
	}
}

void ASnakePawn::HandleMoveUpIA(const FInputActionInstance& InputActionInstance)
{
	if (InputActionInstance.GetValue().IsNonZero())
	{
		// Can't change direction up/down without first going left or right.
		if (FMath::IsNearlyZero(MoveDirection.X))
		{
			const float Amount = InputActionInstance.GetValue().Get<float>();
			PreviousDirection = MoveDirection;
			MoveDirection = FVector(Amount, 0.0f, 0.0f);
			bDirectionChanged = true;
		}
	}
}
