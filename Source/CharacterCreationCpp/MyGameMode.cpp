#include "MyGameMode.h"
#include "WarriorPurpleCharacter.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

AMyGameMode::AMyGameMode()
{
    // Set WarriorPurpleCharacter as the default pawn class
    static ConstructorHelpers::FClassFinder<APawn> PawnClassFinder(TEXT("/Script/CharacterCreationCpp.WarriorPurpleCharacter"));
    if (PawnClassFinder.Succeeded())
    {
        DefaultPawnClass = PawnClassFinder.Class;
        UE_LOG(LogTemp, Warning, TEXT("MyGameMode: Successfully set WarriorPurpleCharacter as default pawn"));
    }
    else
    {
        // Fallback: Try to load the class dynamically
        DefaultPawnClass = AWarriorPurpleCharacter::StaticClass();
        UE_LOG(LogTemp, Warning, TEXT("MyGameMode: Set WarriorPurpleCharacter using StaticClass()"));
    }
    
    // Log the default pawn class for debugging
    if (DefaultPawnClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("MyGameMode: Default Pawn Class is %s"), *DefaultPawnClass->GetName());
    }
}

void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    UE_LOG(LogTemp, Warning, TEXT("MyGameMode: Game mode started with WarriorPurpleCharacter as default pawn"));
    
    // Verify the default pawn class is set
    if (DefaultPawnClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("MyGameMode: Using pawn class: %s"), *DefaultPawnClass->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("MyGameMode: No default pawn class set!"));
    }
}

void AMyGameMode::InitGameState()
{
    Super::InitGameState();
    
    // Additional initialization if needed
    UE_LOG(LogTemp, Warning, TEXT("MyGameMode: Game state initialized"));
}
