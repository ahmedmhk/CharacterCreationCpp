// Generated character class for Warrior_Red
#include "WarriorRedCharacter.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "CharacterCreationCommandlet/CharacterCreationLog.h"

AWarriorRedCharacter::AWarriorRedCharacter()
{
	// Character is already set up by parent class
	// Load animations in constructor for editor preview
	LoadAnimations();
}

void AWarriorRedCharacter::LoadAnimations()
{
	UE_LOG(LogCharacterCreation, Warning, TEXT("Loading animations for WarriorRedCharacter"));
	
	// Load animations specific to Warrior_Red
	IdleAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/Idle_Warrior_Red"));
	MoveAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/Move_Warrior_Red"));
	AttackUpAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackUpwards_Warrior_Red"));
	AttackDownAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackDownwards_Warrior_Red"));
	AttackSideAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackSideways_Warrior_Red"));
	AttackUp2Animation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackUpwards2_Warrior_Red"));
	AttackDown2Animation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackDownwards2_Warrior_Red"));
	AttackSide2Animation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackSideways2_Warrior_Red"));
	
	// Set initial animation
	if (GetSprite() && IdleAnimation)
	{
		GetSprite()->SetFlipbook(IdleAnimation);
		UE_LOG(LogCharacterCreation, Warning, TEXT("✓ Set initial animation for WarriorRedCharacter"));
	}
	else
	{
		UE_LOG(LogCharacterCreation, Warning, TEXT("Failed to set animation - Sprite: %s, IdleAnimation: %s"),
			GetSprite() ? TEXT("Valid") : TEXT("NULL"),
			IdleAnimation ? TEXT("Valid") : TEXT("NULL"));
	}
}
