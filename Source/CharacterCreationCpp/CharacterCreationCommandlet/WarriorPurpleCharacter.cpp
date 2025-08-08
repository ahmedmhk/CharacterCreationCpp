// Generated character class for Warrior_Purple
#include "WarriorPurpleCharacter.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "CharacterCreationLog.h"

AWarriorPurpleCharacter::AWarriorPurpleCharacter()
{
	// Character is already set up by parent class
	// Asset loading moved to BeginPlay() to follow Unreal best practices
}

void AWarriorPurpleCharacter::LoadAnimations()
{
	UE_LOG(LogCharacterCreation, Warning, TEXT("=== WarriorPurpleCharacter::LoadAnimations() CALLED - CORRECT! ==="));
	
	// Load animations specific to Warrior_Purple
	IdleAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/Idle_Warrior_Purple"));
	MoveAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/Move_Warrior_Purple"));
	AttackUpAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackUpwards_Warrior_Purple"));
	AttackDownAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackDownwards_Warrior_Purple"));
	AttackSideAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackSideways_Warrior_Purple"));
	AttackUp2Animation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackUpwards2_Warrior_Purple"));
	AttackDown2Animation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackDownwards2_Warrior_Purple"));
	AttackSide2Animation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackSideways2_Warrior_Purple"));
	
	UE_LOG(LogCharacterCreation, Warning, TEXT("Loaded animations: Idle=%s, Move=%s"), 
		IdleAnimation ? TEXT("YES") : TEXT("NO"),
		MoveAnimation ? TEXT("YES") : TEXT("NO"));
	
	// Set initial animation
	if (GetSprite() && IdleAnimation)
	{
		GetSprite()->SetFlipbook(IdleAnimation);
		UE_LOG(LogCharacterCreation, Warning, TEXT("✓✓✓ SUCCESSFULLY set IdleAnimation flipbook for WarriorPurpleCharacter ✓✓✓"));
	}
	else
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("FAILED to set animation - Sprite: %s, IdleAnimation: %s"),
			GetSprite() ? TEXT("Valid") : TEXT("NULL"),
			IdleAnimation ? TEXT("Valid") : TEXT("NULL"));
	}
}
