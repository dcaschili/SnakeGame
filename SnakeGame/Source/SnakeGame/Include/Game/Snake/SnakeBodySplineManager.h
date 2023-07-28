#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineMeshComponent.h"

#include "SnakeBodySplineManager.generated.h"

class USplineComponent;
class ASnakePawn;
class UStaticMesh;

UCLASS()
class SNAKEGAME_API ASnakeBodySplineManager : public AActor
{
    GENERATED_BODY()
public:
    ASnakeBodySplineManager();
	
	// AActor
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaSeconds) override;
	// !AActor

	void SetSnakePawn(ASnakePawn* InSnakePawn);

	const USplineComponent* GetSplineComponent() const { return SnakeBodySplineComponent; }

private:
	void ClearSplinePoints();
	void AddSplinePointWorldSpace(const FVector& InLocationW);
	void UpdateSplinePointPositionWorldSpace(int32 InIndex, const FVector& InLocationW);

	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Components")
	TObjectPtr<USplineComponent> SnakeBodySplineComponent{};
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Body")
	TEnumAsByte<ESplineMeshAxis::Type> SplineMeshAxis = ESplineMeshAxis::Type::Z;
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|Body")
	TObjectPtr<UStaticMesh> SplineStaticMesh{};
	
	UPROPERTY()
	TArray<USplineMeshComponent*> SnakeBodySplineMeshComponentsList{};
	UPROPERTY()
	TObjectPtr<ASnakePawn> SnakePawn{};
	
};