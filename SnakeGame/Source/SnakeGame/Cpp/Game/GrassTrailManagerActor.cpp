#include "Game/GrassTrailManagerActor.h"

#include "Data/GameConstants.h"
#include "SnakeLog.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Canvas.h"
#include "TimerManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

void AGrassTrailManagerActor::BeginPlay()
{
	Super::BeginPlay();

	if (TextureTarget)
	{
		UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, TextureTarget, RenderTargetCanvas, CanvasSize, RenderTargetContext);

		if (ClearRenderTargetMaterial)
		{
			ClearRenderTargetMI = UMaterialInstanceDynamic::Create(ClearRenderTargetMaterial, this);
		}

		GetWorldTimerManager().SetTimer(
			ClearRenderTargetTimerHandle,
			this,
			&ThisClass::HandleClearRenderTarget,
			ClearRenderTargetRate,
			true);
	}
	else
	{
		GDTUI_LOG(SnakeLogCategoryGame, Error, TEXT("Missing render target texture!"));
		ensure(false);
	}
}

void AGrassTrailManagerActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, RenderTargetContext);

}

void AGrassTrailManagerActor::HandleClearRenderTarget()
{
	if (!ClearRenderTargetMI)
	{
		GDTUI_LOG(SnakeLogCategoryGame, Error, TEXT("Trying to clear grass trail render target without dynamic material intance! Something wrong in the initialization!"));
		if (ClearRenderTargetTimerHandle.IsValid())
		{
			GetWorldTimerManager().ClearTimer(ClearRenderTargetTimerHandle);
		}
		ensure(false);
		return;
	}

	if (!RenderTargetCanvas)
	{
		GDTUI_LOG(SnakeLogCategoryGame, Error, TEXT("Trying to clear grass trail render target without canvas! Something wrong in the initialization!"));
		if (ClearRenderTargetTimerHandle.IsValid())
		{
			GetWorldTimerManager().ClearTimer(ClearRenderTargetTimerHandle);
		}
		ensure(false);
		return;
	}

	const UGameConstants* const GameConstants = UGameConstants::GetGameConstants(this);
	if (ensure(GameConstants))
	{
		// Drawn on the whole canvas
		RenderTargetCanvas->K2_DrawMaterial(ClearRenderTargetMI, FVector2D::ZeroVector, CanvasSize, FVector2D::ZeroVector);
	}
}
