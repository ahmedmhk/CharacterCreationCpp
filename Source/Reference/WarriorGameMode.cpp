#include "WarriorGameMode.h"
#include "WarriorCharacter.h"

AWarriorGameMode::AWarriorGameMode()
{
    // Set default pawn class to our warrior character
    DefaultPawnClass = AWarriorCharacter::StaticClass();
}