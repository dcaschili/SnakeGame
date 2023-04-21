#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ChangeDirectionAction.h"

#include "SnakePawn.generated.h"

class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UInputComponent;
class UEndGameCollisionDetectionComponent;
class UMapOccupancyComponent;
class USnakeBodyPartMoveComponent;
class ASnakeBodyPart;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangeDirectionDelegate, const FChangeDirectionAction&, NewDirectionAction);

UCLASS()
class SNAKEGAME_API ASnakePawn : public APawn
{
    GENERATED_BODY()
public:
	ASnakePawn();

	virtual void Tick(float DeltaSeconds) override;

	FChangeDirectionDelegate OnChangeDirection{};

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	UFUNCTION()
	void HandleMoveRightIA(const FInputActionInstance& InputActionInstance);
	UFUNCTION()
	void HandleMoveUpIA(const FInputActionInstance& InputActionInstance);

	UPROPERTY(EditDefaultsOnly, Category = "Snake|Spawn")
	TSubclassOf<ASnakeBodyPart> SnakeBodyPartClass{};

	UPROPERTY(EditDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComp{};
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<USpringArmComponent> SpringArmComp{};
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<UCameraComponent> CameraComp{};
	UPROPERTY(VisibleAnywhere, Category = "Snake|Components")
	TObjectPtr<UEndGameCollisionDetectionComponent> EndGameCollisionComponent{};
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<UMapOccupancyComponent> MapOccupancyComponent{};
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<USnakeBodyPartMoveComponent> SnakeMovementComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Snake|Inputs")
	TObjectPtr<UInputAction> MoveRightIA{};
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Inputs")
	TObjectPtr<UInputAction> MoveUpIA{};
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Inputs")
	TObjectPtr<UInputMappingContext> InputMappingContext{};
		
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Movement")
	float MaxMovementSpeed = 500.0f;

	FVector				MoveDirection = FVector::RightVector;
	//TOptional<FVector>	PreviousDirection{};
	TOptional<FVector>	PendingMoveDirection{};
	//bool				bDirectionChanged = false;
	int32				TileSize = 0;
	int32				HalfTileSize = 0;
	float				DistanceFromTileCenterTolerance = 0.0f;

#if !UE_BUILD_SHIPPING
	FTimerHandle SnakePositionDebuggerTimerHandle{};
#endif
};