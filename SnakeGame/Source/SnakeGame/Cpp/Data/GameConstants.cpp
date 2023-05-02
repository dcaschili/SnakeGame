#include "Data/GameConstants.h"

#include "SnakeGameInstance.h"
#include "Kismet/GameplayStatics.h"

const UGameConstants* UGameConstants::GetGameConstants(const UObject* InWorldContextObject)
{
	if (USnakeGameInstance* const SnakeGameInstance = Cast<USnakeGameInstance>(UGameplayStatics::GetGameInstance(InWorldContextObject)))
	{
		return SnakeGameInstance->GetGameConstants();
	}
	
	return nullptr;
}
