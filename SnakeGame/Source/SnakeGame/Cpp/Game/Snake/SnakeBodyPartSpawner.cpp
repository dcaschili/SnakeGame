#include "Game/Snake/SnakeBodyPartSpawner.h"

#include "Data/GameConstants.h"
#include "Game/Interfaces/SnakeBodyPartTypeInterface.h"
#include "Game/Snake/SnakePawn.h"
#include "Game/Snake/SnakeBodyPartType.h"
#include "SnakeLog.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "Game/Snake/SnakeBodyPart.h"


ASnakeBodyPartSpawner::ASnakeBodyPartSpawner()
	: Super()
{
	BoxCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box collision component."));	
	RootComponent = BoxCollisionComponent;

	static ConstructorHelpers::FObjectFinder<UGameConstants> DefaultAsset(TEXT("/Game/Data/GameConstants"));
	const UGameConstants* GameConstants = DefaultAsset.Object;
	if (ensure(GameConstants))
	{
		const float BoxExtent = GameConstants->TileSize / 2.0f;
		BoxCollisionComponent->SetBoxExtent({ BoxExtent, BoxExtent, BoxExtent });
	}
}

void ASnakeBodyPartSpawner::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	
	if (bSpawnCompleted) return;

	if (ensure(SnakeBodyPartClass))
	{
		if (OtherActor)
		{
			if (ASnakePawn* const SnakePawn = Cast<ASnakePawn>(OtherActor))
			{
				if (SnakePawn->GetSnakeBodyPartsCount() == 0)
				{
					// No body, just head. No need to look for tail
					GDTUI_SHORT_LOG(SnakeLogCategorySnakeBody, Verbose, TEXT("Empty snake body, spawning first element!"));

					SpawnBodyPart(SnakePawn, SnakePawn->GetMoveDirection());
				}
			}
			else 
			{
				if (ISnakeBodyPartTypeInterface* Interface = Cast<ISnakeBodyPartTypeInterface>(OtherActor))
				{
					if(Interface->IsSnakeTail())
					{
						GDTUI_SHORT_LOG(SnakeLogCategorySnakeBody, Verbose, TEXT("Snake's tail found, spawn new body part!"));
						
						if (ASnakeBodyPart* const CurrentTail = Cast<ASnakeBodyPart>(OtherActor))
						{
							if (ASnakePawn* const TailSnakePawn = CurrentTail->GetSnakePawn())
							{
								CurrentTail->SetSnakeBodyPartType(ESnakeBodyPartType::kBody);
								SpawnBodyPart(TailSnakePawn, CurrentTail->GetMoveDirection(), CurrentTail->GetChangeDirectionQueue());
							}
							else
							{
								GDTUI_LOG(SnakeLogCategorySnakeBody, Error, TEXT("Current body tail doesn't have a valid reference to the snake pawn!"));
								ensure(false);
							}
						}
						else
						{
							GDTUI_LOG(SnakeLogCategorySnakeBody, Error, TEXT("Unhandle case! Found a snake body type tail not on a snake body part!"));
							ensure(false);
						}
					}
				}
			}
		}
	}

	if (bSpawnCompleted)
	{
		Destroy();
	}
}

void ASnakeBodyPartSpawner::SetBodyPartToSpawnCount(int32 InBodyPartToSpawnCount)
{
	BodyPartToSpawnCount = InBodyPartToSpawnCount;
}

void ASnakeBodyPartSpawner::BeginPlay()
{
	Super::BeginPlay();

	const UGameConstants* const GameConstants = UGameConstants::GetGameConstants(this);
	ensure(GameConstants);

	NoCollisionBodyPartsCount = GameConstants ? GameConstants->SnakeNoCollisionBodySize : 3;
	BodyPartStartingHeight = GameConstants ? GameConstants->BodySpawnHeight : 25.0f;
}

void ASnakeBodyPartSpawner::SpawnBodyPart(ASnakePawn* InSnakePawn, const FVector& InMoveDirection, const TArray<FChangeDirectionAction>& InChangeDirectionQueue /* = {}*/)
{
	if (ensure(InSnakePawn) && ensure(SnakeBodyPartClass))
	{
		UWorld* const World = GetWorld();
		if (ensure(World))
		{
			FVector SpawnLocation = GetActorLocation();
			SpawnLocation.Z = BodyPartStartingHeight;
			ASnakeBodyPart* const SnakeBodyPart = World->SpawnActor<ASnakeBodyPart>(SnakeBodyPartClass, SpawnLocation, FRotator::ZeroRotator);
			if (ensure(SnakeBodyPart))
			{
				SnakeBodyPart->SetSnakeBodyPartType(ESnakeBodyPartType::kTail);

				SnakeBodyPart->SetSnakePawn(InSnakePawn);
				SnakeBodyPart->SetMoveDir(InMoveDirection);
				SnakeBodyPart->SetChangeDirQueue(InChangeDirectionQueue);

				if (InSnakePawn->GetSnakeBodyPartsCount() <= NoCollisionBodyPartsCount)
				{
					SnakeBodyPart->SetTriggerEndGameOverlapEvent(false);
				}

				InSnakePawn->AddSnakeBodyPart(SnakeBodyPart);
			
				BodyPartToSpawnCount--;
				bSpawnCompleted = BodyPartToSpawnCount <= 0;
			}
		}
	}
}
