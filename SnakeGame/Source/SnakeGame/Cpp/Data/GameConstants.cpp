#include "Data/GameConstants.h"

#include "SnakeGameInstance.h"
#include "Kismet/GameplayStatics.h"

const UGameConstants* UGameConstants::GetGameConstants(const UObject* InWorldContextObject)
{
	USnakeGameInstance* const SnakeGameInstance = Cast<USnakeGameInstance>(UGameplayStatics::GetGameInstance(InWorldContextObject));
	check(SnakeGameInstance);

	return SnakeGameInstance->GetGameConstants();
}
