#include "SpriteProcessorCommand.h"
#include "HAL/IConsoleManager.h"
#include "CharacterCreationLog.h"

void FSpriteProcessorCommand::RegisterCommands()
{
	// Console command is now registered in module startup
}

void FSpriteProcessorCommand::ProcessSpriteSheet(const TArray<FString>& Args)
{
	UE_LOG(LogCharacterCreation, Error, TEXT("!!! CRITICAL !!! ProcessSpriteSheet FUNCTION CALLED !!!"));
	UE_LOG(LogCharacterCreation, Error, TEXT("!!! EXECUTION CONFIRMED !!! FUNCTION IS RUNNING !!!"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("=== Processing Warrior_Blue Sprite Sheet ==="));
	
	// Create processor instance
	UE_LOG(LogCharacterCreation, Warning, TEXT("Creating USpriteSheetProcessor instance..."));
	USpriteSheetProcessor* Processor = NewObject<USpriteSheetProcessor>();
	
	if (!Processor)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("CRITICAL ERROR: Failed to create USpriteSheetProcessor instance"));
		return;
	}
	
	UE_LOG(LogCharacterCreation, Warning, TEXT("USpriteSheetProcessor instance created successfully"));

	// Configure sprite sheet info
	UE_LOG(LogCharacterCreation, Warning, TEXT("Configuring sprite sheet info..."));
	FSpriteSheetInfo Info;
	Info.Columns = 6;
	Info.Rows = 8;
	Info.SourceTexturePath = TEXT("/Game/RawAssets/");
	Info.DestinationPath = TEXT("/Game/Sprites/");
	
	UE_LOG(LogCharacterCreation, Warning, TEXT("Processor Configuration:"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("- Grid: %d columns x %d rows"), Info.Columns, Info.Rows);
	UE_LOG(LogCharacterCreation, Warning, TEXT("- Source: %s"), *Info.SourceTexturePath);
	UE_LOG(LogCharacterCreation, Warning, TEXT("- Destination: %s"), *Info.DestinationPath);
	UE_LOG(LogCharacterCreation, Warning, TEXT(""));
	
	// Process the sprite sheet
	UE_LOG(LogCharacterCreation, Warning, TEXT("About to call ProcessSpriteSheet(\"Warrior_Blue\", Info)..."));
	
	bool bSuccess = false;
	try 
	{
		UE_LOG(LogCharacterCreation, Warning, TEXT("CALLING Processor->ProcessSpriteSheet now..."));
		bSuccess = Processor->ProcessSpriteSheet(TEXT("Warrior_Blue"), Info);
		UE_LOG(LogCharacterCreation, Warning, TEXT("ProcessSpriteSheet call returned: %s"), bSuccess ? TEXT("TRUE") : TEXT("FALSE"));
	}
	catch (...)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("EXCEPTION caught during ProcessSpriteSheet call!"));
		bSuccess = false;
	}

	if (bSuccess)
	{
		UE_LOG(LogCharacterCreation, Warning, TEXT(""));
		UE_LOG(LogCharacterCreation, Warning, TEXT("✓ SUCCESS! Sprite sheet processed successfully"));
		UE_LOG(LogCharacterCreation, Warning, TEXT(""));
		UE_LOG(LogCharacterCreation, Warning, TEXT("Generated Assets:"));
		UE_LOG(LogCharacterCreation, Warning, TEXT("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"));
		UE_LOG(LogCharacterCreation, Warning, TEXT("📁 Sprites: /Game/Sprites/ (48 individual sprites)"));
		UE_LOG(LogCharacterCreation, Warning, TEXT("🎬 Animations: /Game/Animations/ (8 flipbook animations)"));
		UE_LOG(LogCharacterCreation, Warning, TEXT(""));
		UE_LOG(LogCharacterCreation, Warning, TEXT("Animation Details:"));
		UE_LOG(LogCharacterCreation, Warning, TEXT("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"));
		UE_LOG(LogCharacterCreation, Warning, TEXT("🏃 1. Idle (6 frames @ 12fps)"));
		UE_LOG(LogCharacterCreation, Warning, TEXT("🚶 2. Move (6 frames @ 12fps)"));
		UE_LOG(LogCharacterCreation, Warning, TEXT("⚔️  3. AttackSideways (6 frames @ 12fps)"));
		UE_LOG(LogCharacterCreation, Warning, TEXT("⚔️  4. AttackSideways2 (6 frames @ 12fps)"));
		UE_LOG(LogCharacterCreation, Warning, TEXT("🗡️  5. AttackDownwards (6 frames @ 12fps)"));
		UE_LOG(LogCharacterCreation, Warning, TEXT("🗡️  6. AttackDownwards2 (6 frames @ 12fps)"));
		UE_LOG(LogCharacterCreation, Warning, TEXT("⬆️  7. AttackUpwards (6 frames @ 12fps)"));
		UE_LOG(LogCharacterCreation, Warning, TEXT("⬆️  8. AttackUpwards2 (6 frames @ 12fps)"));
		UE_LOG(LogCharacterCreation, Warning, TEXT(""));
		UE_LOG(LogCharacterCreation, Warning, TEXT("📋 Check the Content Browser to see all generated assets!"));
	}
	else
	{
		UE_LOG(LogCharacterCreation, Error, TEXT(""));
		UE_LOG(LogCharacterCreation, Error, TEXT("✗ FAILED! Sprite sheet processing unsuccessful"));
		UE_LOG(LogCharacterCreation, Error, TEXT(""));
		UE_LOG(LogCharacterCreation, Error, TEXT("Troubleshooting:"));
		UE_LOG(LogCharacterCreation, Error, TEXT("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"));
		UE_LOG(LogCharacterCreation, Error, TEXT("1. Verify Warrior_Blue.png exists in RawAssets folder"));
		UE_LOG(LogCharacterCreation, Error, TEXT("2. Check file permissions on the PNG file"));
		UE_LOG(LogCharacterCreation, Error, TEXT("3. Ensure PNG format is valid"));
		UE_LOG(LogCharacterCreation, Error, TEXT("4. Verify grid dimensions match (6x8)"));
	}
	
	UE_LOG(LogCharacterCreation, Warning, TEXT(""));
	UE_LOG(LogCharacterCreation, Warning, TEXT("=== Sprite Processing Complete ==="));
}