#include "DirectSpriteProcessor.h"

ADirectSpriteProcessor::ADirectSpriteProcessor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADirectSpriteProcessor::BeginPlay()
{
	Super::BeginPlay();
	
	ProcessSpriteSheetDirectly();
}

void ADirectSpriteProcessor::ProcessSpriteSheetDirectly()
{
	UE_LOG(LogTemp, Warning, TEXT("=== Direct Sprite Processing Demo ==="));
	
	// Create processor instance (Usage Option 2)
	USpriteSheetProcessor* Processor = NewObject<USpriteSheetProcessor>();
	
	if (!Processor)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create SpriteSheetProcessor"));
		return;
	}

	// Configure sprite sheet info
	FSpriteSheetInfo Info;
	Info.Columns = 6;
	Info.Rows = 8;
	Info.SourceTexturePath = TEXT("/Game/RawAssets/");
	Info.DestinationPath = TEXT("/Game/Sprites/");
	
	UE_LOG(LogTemp, Log, TEXT("Processor created successfully"));
	UE_LOG(LogTemp, Log, TEXT("Grid configuration: %d columns x %d rows"), Info.Columns, Info.Rows);
	UE_LOG(LogTemp, Log, TEXT("Processing sprite sheet: Warrior_Blue"));
	
	// Process the sprite sheet
	bool bSuccess = Processor->ProcessSpriteSheet(TEXT("Warrior_Blue"), Info);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("✓ SUCCESS: Sprite sheet processed!"));
		UE_LOG(LogTemp, Warning, TEXT("Generated Assets:"));
		UE_LOG(LogTemp, Warning, TEXT("- 48 individual sprites in /Game/Sprites/"));
		UE_LOG(LogTemp, Warning, TEXT("- 8 flipbook animations in /Game/Animations/"));
		UE_LOG(LogTemp, Warning, TEXT(""));
		UE_LOG(LogTemp, Warning, TEXT("Animation List:"));
		UE_LOG(LogTemp, Warning, TEXT("1. Idle (6 frames @ 12fps)"));
		UE_LOG(LogTemp, Warning, TEXT("2. Move (6 frames @ 12fps)"));
		UE_LOG(LogTemp, Warning, TEXT("3. AttackSideways (6 frames @ 12fps)"));
		UE_LOG(LogTemp, Warning, TEXT("4. AttackSideways2 (6 frames @ 12fps)"));
		UE_LOG(LogTemp, Warning, TEXT("5. AttackDownwards (6 frames @ 12fps)"));
		UE_LOG(LogTemp, Warning, TEXT("6. AttackDownwards2 (6 frames @ 12fps)"));
		UE_LOG(LogTemp, Warning, TEXT("7. AttackUpwards (6 frames @ 12fps)"));
		UE_LOG(LogTemp, Warning, TEXT("8. AttackUpwards2 (6 frames @ 12fps)"));
		UE_LOG(LogTemp, Warning, TEXT(""));
		UE_LOG(LogTemp, Warning, TEXT("Check the Content Browser to see the generated assets!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("✗ FAILED: Sprite sheet processing failed"));
		UE_LOG(LogTemp, Error, TEXT("Check that Warrior_Blue.png exists in RawAssets folder"));
	}
	
	UE_LOG(LogTemp, Warning, TEXT("=== Direct Processing Complete ==="));
}