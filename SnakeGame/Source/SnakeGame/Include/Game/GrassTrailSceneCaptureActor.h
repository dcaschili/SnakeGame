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
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|GrassTrail")
	TObjectPtr<UTextureRenderTarget2D> TextureTarget{};
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|GrassTrail")
	TObjectPtr<USceneCaptureComponent2D> SceneCaptureComponent2D{};
	UPROPERTY(EditDefaultsOnly, Category = "SnakeGame|GrassTrail")
	FName GrassTrailEntitiesTag = TEXT("GrassTrail");
};