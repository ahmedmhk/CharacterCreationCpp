#include "CharacterCreationCommandlet.h"
#include "CharacterCreationLog.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

UCharacterCreationCommandlet::UCharacterCreationCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
	ShowErrorCount = true;
	HelpDescription = TEXT("Process sprite sheets for character creation");
	HelpUsage = TEXT("CharacterCreationCommandlet -texture=<TextureName> [-columns=<Columns>] [-rows=<Rows>] [-source=<SourcePath>] [-dest=<DestPath>]");
}

int32 UCharacterCreationCommandlet::Main(const FString& Params)
{
	UE_LOG(LogCharacterCreation, Warning, TEXT("=== Character Creation Commandlet Started ==="));
	UE_LOG(LogCharacterCreation, Warning, TEXT("Parameters: %s"), *Params);

	// Parse command line parameters
	FString TextureName;
	if (!FParse::Value(*Params, TEXT("texture="), TextureName) && !FParse::Value(*Params, TEXT("t="), TextureName))
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Missing required parameter: texture"));
		PrintUsage();
		return 1;
	}

	// Parse optional parameters with defaults
	FSpriteSheetInfo SpriteInfo;
	int32 Columns = 6, Rows = 8;
	FString SourcePath = TEXT("/Game/RawAssets/");
	FString DestPath = TEXT("/Game/Sprites/");

	FParse::Value(*Params, TEXT("columns="), Columns);
	FParse::Value(*Params, TEXT("c="), Columns);
	FParse::Value(*Params, TEXT("rows="), Rows);
	FParse::Value(*Params, TEXT("r="), Rows);
	FParse::Value(*Params, TEXT("source="), SourcePath);
	FParse::Value(*Params, TEXT("s="), SourcePath);
	FParse::Value(*Params, TEXT("dest="), DestPath);
	FParse::Value(*Params, TEXT("d="), DestPath);

	SpriteInfo.Columns = Columns;
	SpriteInfo.Rows = Rows;
	SpriteInfo.SourceTexturePath = SourcePath;
	SpriteInfo.DestinationPath = DestPath;

	UE_LOG(LogCharacterCreation, Warning, TEXT("Processing texture: %s"), *TextureName);
	UE_LOG(LogCharacterCreation, Warning, TEXT("Grid: %dx%d"), SpriteInfo.Columns, SpriteInfo.Rows);
	UE_LOG(LogCharacterCreation, Warning, TEXT("Source: %s"), *SpriteInfo.SourceTexturePath);
	UE_LOG(LogCharacterCreation, Warning, TEXT("Destination: %s"), *SpriteInfo.DestinationPath);

	bool bSuccess = ProcessSpriteSheetFromCommandline(TextureName, SpriteInfo);

	if (bSuccess)
	{
		PrintSuccess(TextureName);
		UE_LOG(LogCharacterCreation, Warning, TEXT("=== Character Creation Commandlet Completed Successfully ==="));
		return 0;
	}
	else
	{
		PrintFailure(TextureName);
		UE_LOG(LogCharacterCreation, Error, TEXT("=== Character Creation Commandlet Failed ==="));
		return 1;
	}
}

bool UCharacterCreationCommandlet::ProcessSpriteSheetFromCommandline(const FString& TextureName, const FSpriteSheetInfo& SpriteInfo)
{
	UE_LOG(LogCharacterCreation, Warning, TEXT("Creating USpriteSheetProcessor instance..."));
	USpriteSheetProcessor* Processor = NewObject<USpriteSheetProcessor>();
	
	if (!Processor)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("CRITICAL ERROR: Failed to create USpriteSheetProcessor instance"));
		return false;
	}
	
	UE_LOG(LogCharacterCreation, Warning, TEXT("USpriteSheetProcessor instance created successfully"));

	UE_LOG(LogCharacterCreation, Warning, TEXT("About to call ProcessSpriteSheet(\"%s\", SpriteInfo)..."), *TextureName);
	
	bool bSuccess = false;
	try 
	{
		UE_LOG(LogCharacterCreation, Warning, TEXT("CALLING Processor->ProcessSpriteSheet now..."));
		bSuccess = Processor->ProcessSpriteSheet(TextureName, SpriteInfo);
		UE_LOG(LogCharacterCreation, Warning, TEXT("ProcessSpriteSheet call returned: %s"), bSuccess ? TEXT("TRUE") : TEXT("FALSE"));
	}
	catch (...)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("EXCEPTION caught during ProcessSpriteSheet call!"));
		bSuccess = false;
	}

	return bSuccess;
}

void UCharacterCreationCommandlet::PrintUsage() const
{
	UE_LOG(LogCharacterCreation, Warning, TEXT(""));
	UE_LOG(LogCharacterCreation, Warning, TEXT("Usage: CharacterCreationCommandlet -texture=<TextureName> [options]"));
	UE_LOG(LogCharacterCreation, Warning, TEXT(""));
	UE_LOG(LogCharacterCreation, Warning, TEXT("Required Parameters:"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  -texture=<name>  | -t=<name>     Texture name (without .png extension)"));
	UE_LOG(LogCharacterCreation, Warning, TEXT(""));
	UE_LOG(LogCharacterCreation, Warning, TEXT("Optional Parameters:"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  -columns=<num>   | -c=<num>      Number of columns (default: 6)"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  -rows=<num>      | -r=<num>      Number of rows (default: 8)"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  -source=<path>   | -s=<path>     Source texture path (default: /Game/RawAssets/)"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  -dest=<path>     | -d=<path>     Destination path (default: /Game/Sprites/)"));
	UE_LOG(LogCharacterCreation, Warning, TEXT(""));
	UE_LOG(LogCharacterCreation, Warning, TEXT("Example:"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  CharacterCreationCommandlet -texture=Warrior_Blue"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  CharacterCreationCommandlet -t=Warrior_Blue -c=6 -r=8"));
	UE_LOG(LogCharacterCreation, Warning, TEXT(""));
}

void UCharacterCreationCommandlet::PrintSuccess(const FString& TextureName) const
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

void UCharacterCreationCommandlet::PrintFailure(const FString& TextureName) const
{
	UE_LOG(LogCharacterCreation, Error, TEXT(""));
	UE_LOG(LogCharacterCreation, Error, TEXT("✗ FAILED! Sprite sheet processing unsuccessful"));
	UE_LOG(LogCharacterCreation, Error, TEXT(""));
	UE_LOG(LogCharacterCreation, Error, TEXT("Troubleshooting:"));
	UE_LOG(LogCharacterCreation, Error, TEXT("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"));
	UE_LOG(LogCharacterCreation, Error, TEXT("1. Verify %s.png exists in RawAssets folder"), *TextureName);
	UE_LOG(LogCharacterCreation, Error, TEXT("2. Check file permissions on the PNG file"));
	UE_LOG(LogCharacterCreation, Error, TEXT("3. Ensure PNG format is valid"));
	UE_LOG(LogCharacterCreation, Error, TEXT("4. Verify grid dimensions match the texture"));
}