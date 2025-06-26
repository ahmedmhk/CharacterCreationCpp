#include "SpriteProcessorCommand.h"
#include "HAL/IConsoleManager.h"

static FAutoConsoleCommand ProcessSpriteSheetCommand(
	TEXT("ProcessSpriteSheet"),
	TEXT("Process the Warrior_Blue sprite sheet using Usage Option 2"),
	FConsoleCommandWithArgsDelegate::CreateStatic(FSpriteProcessorCommand::ProcessSpriteSheet)
);

void FSpriteProcessorCommand::RegisterCommands()
{
	// Commands are auto-registered via FAutoConsoleCommand
}

void FSpriteProcessorCommand::ProcessSpriteSheet(const TArray<FString>& Args)
{
	UE_LOG(LogTemp, Warning, TEXT("=== Usage Option 2: Direct C++ Implementation ==="));
	
	// Create processor instance (this is the main Usage Option 2 code)
	USpriteSheetProcessor* Processor = NewObject<USpriteSheetProcessor>();
	
	if (!Processor)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create USpriteSheetProcessor instance"));
		return;
	}

	// Configure sprite sheet info
	FSpriteSheetInfo Info;
	Info.Columns = 6;
	Info.Rows = 8;
	Info.SourceTexturePath = TEXT("/Game/RawAssets/");
	Info.DestinationPath = TEXT("/Game/Sprites/");
	
	UE_LOG(LogTemp, Warning, TEXT("Processor Configuration:"));
	UE_LOG(LogTemp, Warning, TEXT("- Grid: %d columns x %d rows"), Info.Columns, Info.Rows);
	UE_LOG(LogTemp, Warning, TEXT("- Source: %s"), *Info.SourceTexturePath);
	UE_LOG(LogTemp, Warning, TEXT("- Destination: %s"), *Info.DestinationPath);
	UE_LOG(LogTemp, Warning, TEXT(""));
	
	// Process the sprite sheet (this is the core functionality)
	UE_LOG(LogTemp, Warning, TEXT("Calling ProcessSpriteSheet(\"Warrior_Blue\", Info)..."));
	bool bSuccess = Processor->ProcessSpriteSheet(TEXT("Warrior_Blue"), Info);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT(""));
		UE_LOG(LogTemp, Warning, TEXT("✓ SUCCESS! Sprite sheet processed successfully"));
		UE_LOG(LogTemp, Warning, TEXT(""));
		UE_LOG(LogTemp, Warning, TEXT("Generated Assets:"));
		UE_LOG(LogTemp, Warning, TEXT("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"));
		UE_LOG(LogTemp, Warning, TEXT("📁 Sprites: /Game/Sprites/ (48 individual sprites)"));
		UE_LOG(LogTemp, Warning, TEXT("🎬 Animations: /Game/Animations/ (8 flipbook animations)"));
		UE_LOG(LogTemp, Warning, TEXT(""));
		UE_LOG(LogTemp, Warning, TEXT("Animation Details:"));
		UE_LOG(LogTemp, Warning, TEXT("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"));
		UE_LOG(LogTemp, Warning, TEXT("🏃 1. Idle (6 frames @ 12fps)"));
		UE_LOG(LogTemp, Warning, TEXT("🚶 2. Move (6 frames @ 12fps)"));
		UE_LOG(LogTemp, Warning, TEXT("⚔️  3. AttackSideways (6 frames @ 12fps)"));
		UE_LOG(LogTemp, Warning, TEXT("⚔️  4. AttackSideways2 (6 frames @ 12fps)"));
		UE_LOG(LogTemp, Warning, TEXT("🗡️  5. AttackDownwards (6 frames @ 12fps)"));
		UE_LOG(LogTemp, Warning, TEXT("🗡️  6. AttackDownwards2 (6 frames @ 12fps)"));
		UE_LOG(LogTemp, Warning, TEXT("⬆️  7. AttackUpwards (6 frames @ 12fps)"));
		UE_LOG(LogTemp, Warning, TEXT("⬆️  8. AttackUpwards2 (6 frames @ 12fps)"));
		UE_LOG(LogTemp, Warning, TEXT(""));
		UE_LOG(LogTemp, Warning, TEXT("📋 Check the Content Browser to see all generated assets!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT(""));
		UE_LOG(LogTemp, Error, TEXT("✗ FAILED! Sprite sheet processing unsuccessful"));
		UE_LOG(LogTemp, Error, TEXT(""));
		UE_LOG(LogTemp, Error, TEXT("Troubleshooting:"));
		UE_LOG(LogTemp, Error, TEXT("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"));
		UE_LOG(LogTemp, Error, TEXT("1. Verify Warrior_Blue.png exists in RawAssets folder"));
		UE_LOG(LogTemp, Error, TEXT("2. Check file permissions on the PNG file"));
		UE_LOG(LogTemp, Error, TEXT("3. Ensure PNG format is valid"));
		UE_LOG(LogTemp, Error, TEXT("4. Verify grid dimensions match (6x8)"));
	}
	
	UE_LOG(LogTemp, Warning, TEXT(""));
	UE_LOG(LogTemp, Warning, TEXT("=== Usage Option 2 Demo Complete ==="));
}