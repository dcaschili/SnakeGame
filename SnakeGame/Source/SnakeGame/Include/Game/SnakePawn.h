#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "SnakePawn.generated.h"

class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UInputComponent;
class UEndGameCollisionDetectionComponent;
class UMapOccupancyComponent;

UCLASS()
class SNAKEGAME_API ASnakePawn : public APawn
{
    GENERATED_BODY()
public:
	ASnakePawn();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	UFUNCTION()
	void HandleMoveRightIA(const FInputActionInstance& InputActionInstance);
	UFUNCTION()
	void HandleMoveUpIA(const FInputActionInstance& InputActionInstance);

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

	UPROPERTY(EditDefaultsOnly, Category = "Snake|Inputs")
	TObjectPtr<UInputAction> MoveRightIA{};
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Inputs")
	TObjectPtr<UInputAction> MoveUpIA{};
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Inputs")
	TObjectPtr<UInputMappingContext> InputMappingContext{};
		
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Movement")
	float MaxMovementSpeed = 500.0f;

	FVector				MoveDirection = FVector::RightVector;
	TOptional<FVector>	PreviousDirection{};
	bool				bDirectionChanged = false;
	int32				TileSize = 0;
	int32				HalfTileSize = 0;

#if !UE_BUILD_SHIPPING
	FTimerHandle SnakePositionDebuggerTimerHandle{};
#endif
};