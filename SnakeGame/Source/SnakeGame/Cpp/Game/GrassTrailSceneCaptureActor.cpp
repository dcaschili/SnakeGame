#include "Game/GrassTrailSceneCaptureActor.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Data/GameConstants.h"
#include "SnakeLog.h"
#include "InstancedFoliageActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"

#if !UE_BUILD_SHIPPING
#include "Engine.h"
#endif // !UE_BUILD_SHIPPING

AGrassTrailSceneCaptureActor::AGrassTrailSceneCaptureActor()
	: Super()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent"));
	
}

void AGrassTrailSceneCaptureActor::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(TextureTarget))
	{
		UKismetRenderingLibrary::ClearRenderTarget2D(this, TextureTarget, FLinearColor{ 0.0f, 0.0f, 0.0f, 0.0f });
	}

	if (SceneCaptureComponent2D)
	{
		// Get grass plane size.
		const UGameConstants* const GameConstants = UGameConstants::GetGameConstants(this);
		if (!GameConstants)
		{
			GDTUI_LOG(SnakeLogCategoryGame, Error, TEXT("Can't find game constants!"));
			GDTUI_PRINT_TO_SCREEN_ERROR(TEXT("Can't find game constants!"));
			ensure(false);
			return;
		}
		ensure(GameConstants);
		if (!GameConstants) return;

		SceneCaptureComponent2D->ProjectionType = ECameraProjectionMode::Type::Orthographic;
		SceneCaptureComponent2D->OrthoWidth = GameConstants->GetMapSideSize();
		SceneCaptureComponent2D->TextureTarget = TextureTarget;
		SceneCaptureComponent2D->CompositeMode = ESceneCaptureCompositeMode::SCCM_Additive;
		SceneCaptureComponent2D->ProfilingEventName = TEXT("GrassTrail_SceneCapture");
		SceneCaptureComponent2D->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;

		// Exclude actors from capture
		TArray<AActor*> ToHideActors{};
		UGameplayStatics::GetAllActorsOfClass(this, AInstancedFoliageActor::StaticClass(), ToHideActors);
		for (AActor* const Actor : ToHideActors)
		{
			SceneCaptureComponent2D->HiddenActors.AddUnique(Actor);
		}

		ToHideActors.Empty();
		UGameplayStatics::GetAllActorsWithTag(this, GameConstants->GrassFloorTag, ToHideActors);
		if (ToHideActors.IsEmpty())
		{
			GDTUI_SHORT_LOG(SnakeLogCategoryGame, Warning, TEXT("Missing floor tag in mesh, the scene capture component may not work!"));
			ensure(false);
		}
		for (AActor* const Actor : ToHideActors)
		{
			SceneCaptureComponent2D->HiddenActors.AddUnique(Actor);
		}
	}
}

void AGrassTrailSceneCaptureActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
