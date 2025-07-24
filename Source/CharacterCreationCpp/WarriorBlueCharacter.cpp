// Generated character class for Warrior_Blue
#include "WarriorBlueCharacter.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "CharacterCreationLog.h"

AWarriorBlueCharacter::AWarriorBlueCharacter()
{
	// Character is already set up by parent class
	// Load animations in constructor for editor preview
	LoadAnimationsForCharacter();
}

void AWarriorBlueCharacter::LoadAnimationsForCharacter()
{
	UE_LOG(LogCharacterCreation, Warning, TEXT("Loading animations for WarriorBlueCharacter"));
	
	// Load animations specific to Warrior_Blue
	IdleAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/Idle_Warrior_Blue"));
	MoveAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/Move_Warrior_Blue"));
	AttackUpAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackUpwards_Warrior_Blue"));
	AttackDownAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackDownwards_Warrior_Blue"));
	AttackSideAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackSideways_Warrior_Blue"));
	AttackUp2Animation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackUpwards2_Warrior_Blue"));
	AttackDown2Animation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackDownwards2_Warrior_Blue"));
	AttackSide2Animation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackSideways2_Warrior_Blue"));
	
	// Set initial animation
	if (GetSprite() && IdleAnimation)
	{
		GetSprite()->SetFlipbook(IdleAnimation);
		UE_LOG(LogCharacterCreation, Warning, TEXT("✓ Set initial animation for WarriorBlueCharacter"));
	}
	else
	{
		UE_LOG(LogCharacterCreation, Warning, TEXT("Failed to set animation - Sprite: %s, IdleAnimation: %s"),
			GetSprite() ? TEXT("Valid") : TEXT("NULL"),
			IdleAnimation ? TEXT("Valid") : TEXT("NULL"));
	}
}
