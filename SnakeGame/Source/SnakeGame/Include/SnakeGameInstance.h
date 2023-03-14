#pragma once

#include "Engine/GameInstance.h"

#include "SnakeGameInstance.generated.h"

class UGameConstants;

UCLASS()
class SNAKEGAME_API USnakeGameInstance : public UGameInstance
{
    GENERATED_BODY()
public:

    const UGameConstants* GetGameConstants() const { return GameConstants; }

private:
    UPROPERTY(EditDefaultsOnly, Category="Game Setup")
    TObjectPtr<UGameConstants> GameConstants{};
};