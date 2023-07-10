#pragma once

#include "GameFramework/Actor.h"
#include "Kismet/KismetRenderingLibrary.h"

#include "GrassTrailManagerActor.generated.h"

class UMaterialInterface;
class UTextureRenderTarget2D;
class UCanvas;
class UMaterialInstanceDynamic;

UCLASS()
class SNAKEGAME_API AGrassTrailManagerActor : public AActor
{
    GENERATED_BODY()
protected:
	// AActor
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void HandleClearRenderTarget();

	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|GrassTrail")
	TObjectPtr<class UTextureRenderTarget2D> TextureTarget{};
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|GrassTrail")
	float ClearRenderTargetRate = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|GrassTrail")
	TObjectPtr<UMaterialInterface> ClearRenderTargetMaterial{};

	// Can't use TObjectPtr due to *& requirement (look at beginplay).
	UPROPERTY()
	UCanvas* RenderTargetCanvas{};

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ClearRenderTargetMI{};

	FVector2D CanvasSize{};
	FDrawToRenderTargetContext RenderTargetContext{};
	FTimerHandle ClearRenderTargetTimerHandle{};
};