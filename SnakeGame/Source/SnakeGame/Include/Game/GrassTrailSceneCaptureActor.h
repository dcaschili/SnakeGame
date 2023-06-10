#pragma once

#include "GameFramework/Actor.h"

#include "GrassTrailSceneCaptureActor.generated.h"

class USceneCaptureComponent2D;
class UTextureRenderTarget2D;

UCLASS()
class SNAKEGAME_API AGrassTrailSceneCaptureActor : public AActor
{
    GENERATED_BODY()
public:
	AGrassTrailSceneCaptureActor();

protected:
	
	virtual void BeginPlay();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

private:
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|SceneCapture")
	TObjectPtr<class UTextureRenderTarget2D> TextureTarget{};
	UPROPERTY()
	TObjectPtr<USceneCaptureComponent2D> SceneCaptureComponent2D{};
};