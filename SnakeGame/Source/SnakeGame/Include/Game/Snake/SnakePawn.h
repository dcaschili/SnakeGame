#pragma once

#include "GameFramework/Pawn.h"
#include "Game/Interfaces/SnakeBodyPartTypeInterface.h"
#include "Game/ChangeDirectionAction.h"

#include "SnakePawn.generated.h"

class UStaticMeshComponent;
class UInputAction;
class UInputComponent;
class USnakeBodyPartMoveComponent;
class ASnakeBodyPart;
class ASnakeBodyPartSpawner;
class UEndGameOverlapDetectionComponent;
class USnakeChangeDirectionAudioComponent;
class UMapOccupancyComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangeDirectionDelegate, const FChangeDirectionAction&, NewDirectionAction);

UCLASS()
class SNAKEGAME_API ASnakePawn : public APawn, public ISnakeBodyPartTypeInterface
{
    GENERATED_BODY()
public:
	ASnakePawn();

	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaSeconds) override;

	// ISnakeBodyPartTypeInterface
	virtual void				SetSnakeBodyPartType(ESnakeBodyPartType InBodyPartType) override { BodyPartType = BodyPartType; }
	virtual ESnakeBodyPartType	GetSnakeBodyPartType() const override { return BodyPartType; };

	const ASnakeBodyPart* GetSnakeBodyPartAtIndex(int32 InBodyPartIndex) const;
	int32 GetSnakeBodyPartsCount() const { return SnakeBody.Num(); }
	void AddSnakeBodyPart(ASnakeBodyPart* InSnakeBodyPart);

	FVector GetMoveDirection() const; 

	FORCEINLINE UEndGameOverlapDetectionComponent* GetEndGameOverlapDetectionComponent() const { return EndGameOverlapComponent; }

	FChangeDirectionDelegate OnChangeDirection{};

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void BindEvents();
	virtual void UnbindEvents();

	UPROPERTY(EditDefaultsOnly, Category = "Snake|Spawn")
	TSubclassOf<ASnakeBodyPartSpawner> SnakeBodyPartSpawnerClass{};

private:
	UFUNCTION()
	void HandleCollectibleCollected(const FVector& InCollectibleLocation);
	UFUNCTION()
	void HandleMoveRightIA(const FInputActionInstance& InputActionInstance);
	UFUNCTION()
	void HandleMoveUpIA(const FInputActionInstance& InputActionInstance);

	UPROPERTY(EditDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<UEndGameOverlapDetectionComponent> EndGameOverlapComponent{};
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<UMapOccupancyComponent> MapOccupancyComponent{};
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComp{};	
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<USnakeBodyPartMoveComponent> SnakeMovementComponent{};
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Components")
	TObjectPtr<USnakeChangeDirectionAudioComponent> SnakeChangeDirectionAudioComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Snake|Inputs")
	TObjectPtr<UInputAction> MoveRightIA{};
	UPROPERTY(EditDefaultsOnly, Category = "Snake|Inputs")
	TObjectPtr<UInputAction> MoveUpIA{};

	UPROPERTY(EditDefaultsOnly, Category = "Snake|Body", meta=(ClampMin=1, UIMin=1))
	int32 InitialBodyPartsCount = 3;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess=true))
	ESnakeBodyPartType BodyPartType{};

	UPROPERTY()
	TArray<ASnakeBodyPart*> SnakeBody{};

	
	FVector				MoveDirection = FVector::RightVector;
	TOptional<FVector>	PendingMoveDirection{};
	int32				TileSize = 0;
	int32				HalfTileSize = 0;
	float				DistanceFromTileCenterTolerance = 0.0f;
	/* 
		After a direction change, I should wait for the new tile center 
		before aplying a new change. The user can change direction but it 
		isn't applied until the new tile center.
	*/
	bool				bChangeDirectionEnabled = true;


#if !UE_BUILD_SHIPPING
	FTimerHandle SnakePositionDebuggerTimerHandle{};
#endif

};