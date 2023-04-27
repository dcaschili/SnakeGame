#include "Game/SnakeBodyPartSpawner.h"

#include "Data/GameConstants.h"
#include "Game/SnakeBodyPartTypeInterface.h"
#include "Game/SnakePawn.h"
#include "Game/SnakeBodyPartType.h"
#include "SnakeLog.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "Game/SnakeBodyPart.h"


ASnakeBodyPartSpawner::ASnakeBodyPartSpawner()
	: Super()
{
	BoxCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box collision component."));	
	RootComponent = BoxCollisionComponent;

	const UGameConstants* GameConstants = UGameConstants::GetGameConstants(this);
	if (!GameConstants)
	{
		static ConstructorHelpers::FObjectFinder<UGameConstants> DefaultAsset(TEXT("/Game/Data/GameConstants"));
		GameConstants = DefaultAsset.Object;
		check(GameConstants);
	}

	if (GameConstants)
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
					UE_LOG(SnakeLogCategorySnakeBody, Verbose, TEXT("ASnakeBodyPartSpawner - Empty snake body, spawning first element!"));

					SpawnBodyPart(SnakePawn, SnakePawn->GetMoveDirection());
				}
			}
			else 
			{
				if (ISnakeBodyPartTypeInterface* Interface = Cast<ISnakeBodyPartTypeInterface>(OtherActor))
				{
					ESnakeBodyPartType BodyPartType = Interface->GetSnakeBodyPartType();
					if (BodyPartType == ESnakeBodyPartType::kTail)
					{
						UE_LOG(SnakeLogCategorySnakeBody, Verbose, TEXT("ASnakeBodyPartSpawner - Snake's tail found, spawn new body part!"));
						
						if (ASnakeBodyPart* const CurrentTail = Cast<ASnakeBodyPart>(OtherActor))
						{
							if (ASnakePawn* const TailSnakePawn = CurrentTail->GetSnakePawn())
							{
								CurrentTail->SetSnakeBodyPartType(ESnakeBodyPartType::kBody);
								SpawnBodyPart(TailSnakePawn, CurrentTail->GetMoveDirection(), CurrentTail->GetChangeDirectionQueue());
							}
							else
							{
								UE_LOG(SnakeLogCategorySnakeBody, Error, TEXT("Current body tail doesn't have a valid reference to the snake pawn!"));
								ensure(false);
							}
						}
						else
						{
							UE_LOG(SnakeLogCategorySnakeBody, Error, TEXT("Unhandle case! Found a snake body type tail not on a snake body part!"));
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

void ASnakeBodyPartSpawner::SpawnBodyPart(ASnakePawn* InSnakePawn, const FVector& InMoveDirection, const TArray<FChangeDirectionAction>& InChangeDirectionQueue /* = {}*/)
{
	if (ensure(InSnakePawn) && ensure(SnakeBodyPartClass))
	{
		UWorld* const World = GetWorld();
		if (ensure(World))
		{
			ASnakeBodyPart* const SnakeBodyPart = World->SpawnActor<ASnakeBodyPart>(SnakeBodyPartClass, GetActorLocation(), FRotator::ZeroRotator);
			if (ensure(SnakeBodyPart))
			{
				InSnakePawn->AddSnakeBodyPart(SnakeBodyPart);

				SnakeBodyPart->SetSnakeBodyPartType(ESnakeBodyPartType::kTail);
				SnakeBodyPart->SetSnakePawn(InSnakePawn);
				SnakeBodyPart->SetMoveDir(InMoveDirection);
				SnakeBodyPart->SetChangeDirQueue(InChangeDirectionQueue);
			
				bSpawnCompleted = true;
			}
		}
		
	}
}
