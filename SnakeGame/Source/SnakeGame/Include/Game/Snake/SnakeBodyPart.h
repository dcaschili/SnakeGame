#pragma once

#include "GameFramework/Actor.h"
#include "Game/Snake/SnakeBodyPartType.h"
#include "Game/Interfaces/SnakeBodyPartTypeInterface.h"
#include "Game/Interfaces/TriggerEndGameInterface.h"

#include "SnakeBodyPart.generated.h"

class ASnakePawn;
class USnakeMoveComponent;
class UMapOccupancyComponent;
class UBoxComponent;
class UStaticMesh;

UCLASS()
class SNAKEGAME_API ASnakeBodyPart : public AActor, public ISnakeBodyPartTypeInterface, public ITriggerEndGameInterface
{
    GENERATED_BODY()
public:
	ASnakeBodyPart();

	// AActor
#if !UE_BUILD_SHIPPING
	virtual void Tick(float DeltaSeconds) override;
#endif // !UE_BUILD_SHIPPING
	// !AActor

	void		SetMoveDir(const FVector& InMoveDirection);
	FVector		GetMoveDirection() const;
	void		SetSnakePawn(ASnakePawn* InPawnPtr);
	ASnakePawn* GetSnakePawn() const;
	
	USnakeMoveComponent*		GetSnakeMoveComponent() { return BodyPartMoveComp; }
	const USnakeMoveComponent*	GetSnakeMoveComponent() const { return BodyPartMoveComp; }
	
	FORCEINLINE void SetTriggerEndGameOverlapEvent(bool bEnabled) { bTriggerEndGameOverlapEvent = bEnabled; }
	FORCEINLINE bool GetTriggerEndGameOverlapEvent() const { return bTriggerEndGameOverlapEvent; }

	/*
		INTERFACES
	*/

	// ISnakeBodyPartTypeInterface
	virtual void						SetSnakeBodyPartType(ESnakeBodyPartType InBodyPartType) override;
	virtual ESnakeBodyPartType			GetSnakeBodyPartType() const override { return BodyPartType; };

	// ITriggerEndGameInterface
	virtual bool ShouldTriggerEndGame() const override { return GetTriggerEndGameOverlapEvent(); }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void HandleChangeDirectionAction(const FChangeDirectionAction& NewDirectionAction);

	void BindPawnDelegates();
	void UnbindPawnDelegates();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SnakeGame|Body", meta=(AllowPrivateAccess=true))
	ESnakeBodyPartType BodyPartType {};
	UPROPERTY(EditAnywhere, Category = "SnakeGame|Body")
	float SnakeBodyBoxColliderExtent = 40.0f;
	UPROPERTY(EditAnywhere, Category = "SnakeGame|Body|Debug")
	float SnakeBodyBoxColliderThickness = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Components")
	TObjectPtr<UBoxComponent> SnakeBodyPartCollider{};
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Components")
	TObjectPtr<UMapOccupancyComponent> MapOccupancyComponent{};
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Components")
	TObjectPtr<USnakeMoveComponent> BodyPartMoveComp{};


	UPROPERTY()
	TObjectPtr<ASnakePawn>			SnakePawnPtr{};
	
	bool	bTriggerEndGameOverlapEvent = true;
};