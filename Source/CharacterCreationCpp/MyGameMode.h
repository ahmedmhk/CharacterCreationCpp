#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameMode.generated.h"

/**
 * Custom game mode that uses WarriorPurpleCharacter as the default pawn
 */
UCLASS()
class CHARACTERCREATIONCPP_API AMyGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AMyGameMode();

    /** Called when the game starts or when spawned */
    virtual void BeginPlay() override;

protected:
    /** Override to specify the default pawn class */
    virtual void InitGameState() override;
};
