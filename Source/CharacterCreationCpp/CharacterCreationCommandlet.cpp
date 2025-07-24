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
	HelpUsage = TEXT("CharacterCreationCommandlet [-texture=<TextureName>] [-batch] [-createcharacter] [-columns=<Columns>] [-rows=<Rows>] [-source=<SourcePath>] [-dest=<DestPath>]");
}

int32 UCharacterCreationCommandlet::Main(const FString& Params)
{
	UE_LOG(LogCharacterCreation, Warning, TEXT("=== Character Creation Commandlet Started ==="));
	UE_LOG(LogCharacterCreation, Warning, TEXT("Parameters: %s"), *Params);

	// Check for batch mode
	bool bBatchMode = FParse::Param(*Params, TEXT("batch"));
	bool bCreateCharacters = FParse::Param(*Params, TEXT("createcharacter"));

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

	if (bBatchMode)
	{
		UE_LOG(LogCharacterCreation, Warning, TEXT("Running in BATCH MODE"));
		UE_LOG(LogCharacterCreation, Warning, TEXT("Create Characters: %s"), bCreateCharacters ? TEXT("YES") : TEXT("NO"));
		
		bool bSuccess = BatchProcessSpriteSheets(SpriteInfo, bCreateCharacters);
		
		if (bSuccess)
		{
			UE_LOG(LogCharacterCreation, Warning, TEXT("=== Character Creation Commandlet Completed Successfully ==="));
			return 0;
		}
		else
		{
			UE_LOG(LogCharacterCreation, Error, TEXT("=== Character Creation Commandlet Failed ==="));
			return 1;
		}
	}
	else
	{
		// Single texture mode
		FString TextureName;
		if (!FParse::Value(*Params, TEXT("texture="), TextureName) && !FParse::Value(*Params, TEXT("t="), TextureName))
		{
			UE_LOG(LogCharacterCreation, Error, TEXT("Missing required parameter: texture (or use -batch for batch mode)"));
			PrintUsage();
			return 1;
		}

		UE_LOG(LogCharacterCreation, Warning, TEXT("Processing texture: %s"), *TextureName);
		UE_LOG(LogCharacterCreation, Warning, TEXT("Grid: %dx%d"), SpriteInfo.Columns, SpriteInfo.Rows);
		UE_LOG(LogCharacterCreation, Warning, TEXT("Source: %s"), *SpriteInfo.SourceTexturePath);
		UE_LOG(LogCharacterCreation, Warning, TEXT("Destination: %s"), *SpriteInfo.DestinationPath);

		bool bSuccess = ProcessSpriteSheetFromCommandline(TextureName, SpriteInfo);

		if (bSuccess)
		{
			PrintSuccess(TextureName);
			
			// Generate character class if requested
			if (bCreateCharacters)
			{
				FString CharacterName = TextureName.Replace(TEXT("_"), TEXT("")) + TEXT("Character");
				if (GenerateCharacterClass(CharacterName, TextureName))
				{
					UE_LOG(LogCharacterCreation, Warning, TEXT("✓ Generated character class: %s"), *CharacterName);
				}
			}
			
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
	UE_LOG(LogCharacterCreation, Warning, TEXT("Usage: CharacterCreationCommandlet [-texture=<TextureName>] [-batch] [options]"));
	UE_LOG(LogCharacterCreation, Warning, TEXT(""));
	UE_LOG(LogCharacterCreation, Warning, TEXT("Mode Parameters (choose one):"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  -texture=<name>  | -t=<name>     Process single texture (without .png extension)"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  -batch                           Process all PNG files in RawAssets directory"));
	UE_LOG(LogCharacterCreation, Warning, TEXT(""));
	UE_LOG(LogCharacterCreation, Warning, TEXT("Optional Parameters:"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  -createcharacter                 Generate C++ character classes"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  -columns=<num>   | -c=<num>      Number of columns (default: 6)"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  -rows=<num>      | -r=<num>      Number of rows (default: 8)"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  -source=<path>   | -s=<path>     Source texture path (default: /Game/RawAssets/)"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  -dest=<path>     | -d=<path>     Destination path (default: /Game/Sprites/)"));
	UE_LOG(LogCharacterCreation, Warning, TEXT(""));
	UE_LOG(LogCharacterCreation, Warning, TEXT("Examples:"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  CharacterCreationCommandlet -texture=Warrior_Blue"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  CharacterCreationCommandlet -batch -createcharacter"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  CharacterCreationCommandlet -t=Warrior_Blue -c=6 -r=8 -createcharacter"));
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

bool UCharacterCreationCommandlet::BatchProcessSpriteSheets(const FSpriteSheetInfo& SpriteInfo, bool bCreateCharacters)
{
	UE_LOG(LogCharacterCreation, Warning, TEXT(""));
	UE_LOG(LogCharacterCreation, Warning, TEXT("=== Starting Batch Processing ==="));
	
	// Get the RawAssets directory path
	FString RawAssetsPath = FPaths::ProjectDir() / TEXT("RawAssets/");
	
	// Find all PNG files in the directory
	TArray<FString> PNGFiles;
	IFileManager::Get().FindFiles(PNGFiles, *(RawAssetsPath + TEXT("*.png")), true, false);
	
	if (PNGFiles.Num() == 0)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("No PNG files found in RawAssets directory: %s"), *RawAssetsPath);
		return false;
	}
	
	UE_LOG(LogCharacterCreation, Warning, TEXT("Found %d PNG files to process"), PNGFiles.Num());
	
	TArray<FString> ProcessedTextures;
	TArray<FString> GeneratedCharacters;
	bool bAllSuccessful = true;
	
	// Process each PNG file
	for (const FString& PNGFile : PNGFiles)
	{
		FString TextureName = FPaths::GetBaseFilename(PNGFile);
		
		UE_LOG(LogCharacterCreation, Warning, TEXT(""));
		UE_LOG(LogCharacterCreation, Warning, TEXT("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"));
		UE_LOG(LogCharacterCreation, Warning, TEXT("Processing: %s"), *TextureName);
		UE_LOG(LogCharacterCreation, Warning, TEXT("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"));
		
		bool bSuccess = ProcessSpriteSheetFromCommandline(TextureName, SpriteInfo);
		
		if (bSuccess)
		{
			ProcessedTextures.Add(TextureName);
			PrintSuccess(TextureName);
			
			// Generate character class if requested
			if (bCreateCharacters)
			{
				FString CharacterName = TextureName.Replace(TEXT("_"), TEXT("")) + TEXT("Character");
				if (GenerateCharacterClass(CharacterName, TextureName))
				{
					GeneratedCharacters.Add(CharacterName);
					UE_LOG(LogCharacterCreation, Warning, TEXT("✓ Generated character class: %s"), *CharacterName);
				}
				else
				{
					UE_LOG(LogCharacterCreation, Error, TEXT("✗ Failed to generate character class: %s"), *CharacterName);
					bAllSuccessful = false;
				}
			}
		}
		else
		{
			PrintFailure(TextureName);
			bAllSuccessful = false;
		}
	}
	
	// Print summary
	PrintBatchSummary(ProcessedTextures, GeneratedCharacters);
	
	return bAllSuccessful;
}

bool UCharacterCreationCommandlet::GenerateCharacterClass(const FString& CharacterName, const FString& TextureName)
{
	UE_LOG(LogCharacterCreation, Warning, TEXT(""));
	UE_LOG(LogCharacterCreation, Warning, TEXT("Generating character class: %s"), *CharacterName);
	
	bool bHeaderSuccess = WriteCharacterHeaderFile(CharacterName, TextureName);
	bool bSourceSuccess = WriteCharacterSourceFile(CharacterName, TextureName);
	
	if (bHeaderSuccess && bSourceSuccess)
	{
		UE_LOG(LogCharacterCreation, Warning, TEXT("✓ Successfully generated %s.h and %s.cpp"), *CharacterName, *CharacterName);
		UE_LOG(LogCharacterCreation, Warning, TEXT("  NOTE: Project must be recompiled for new classes to be available"));
		return true;
	}
	
	return false;
}

bool UCharacterCreationCommandlet::WriteCharacterHeaderFile(const FString& CharacterName, const FString& TextureName)
{
	FString FilePath = FPaths::ProjectDir() / TEXT("Source/CharacterCreationCpp/") / CharacterName + TEXT(".h");
	
	FString HeaderContent = FString::Printf(TEXT(
		"// Generated character class for %s\n"
		"#pragma once\n"
		"\n"
		"#include \"CoreMinimal.h\"\n"
		"#include \"WarriorCharacter.h\"\n"
		"#include \"%s.generated.h\"\n"
		"\n"
		"UCLASS()\n"
		"class CHARACTERCREATIONCPP_API A%s : public AWarriorCharacter\n"
		"{\n"
		"	GENERATED_BODY()\n"
		"\n"
		"public:\n"
		"	A%s();\n"
		"\n"
		"protected:\n"
		"	virtual void LoadAnimationsForCharacter() override;\n"
		"};\n"
	), *TextureName, *CharacterName, *CharacterName, *CharacterName);
	
	if (FFileHelper::SaveStringToFile(HeaderContent, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		UE_LOG(LogCharacterCreation, Log, TEXT("Created header file: %s"), *FilePath);
		return true;
	}
	else
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Failed to create header file: %s"), *FilePath);
		return false;
	}
}

bool UCharacterCreationCommandlet::WriteCharacterSourceFile(const FString& CharacterName, const FString& TextureName)
{
	FString FilePath = FPaths::ProjectDir() / TEXT("Source/CharacterCreationCpp/") / CharacterName + TEXT(".cpp");
	
	FString SourceContent = FString::Printf(TEXT(
		"// Generated character class for %s\n"
		"#include \"%s.h\"\n"
		"#include \"PaperFlipbook.h\"\n"
		"#include \"PaperFlipbookComponent.h\"\n"
		"#include \"CharacterCreationLog.h\"\n"
		"\n"
		"A%s::A%s()\n"
		"{\n"
		"	// Character is already set up by parent class\n"
		"	// Load animations in constructor for editor preview\n"
		"	LoadAnimationsForCharacter();\n"
		"}\n"
		"\n"
		"void A%s::LoadAnimationsForCharacter()\n"
		"{\n"
		"	UE_LOG(LogCharacterCreation, Warning, TEXT(\"Loading animations for %s\"));\n"
		"	\n"
		"	// Load animations specific to %s\n"
		"	IdleAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT(\"/Game/Animations/Idle_%s\"));\n"
		"	MoveAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT(\"/Game/Animations/Move_%s\"));\n"
		"	AttackUpAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT(\"/Game/Animations/AttackUpwards_%s\"));\n"
		"	AttackDownAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT(\"/Game/Animations/AttackDownwards_%s\"));\n"
		"	AttackSideAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT(\"/Game/Animations/AttackSideways_%s\"));\n"
		"	AttackUp2Animation = LoadObject<UPaperFlipbook>(nullptr, TEXT(\"/Game/Animations/AttackUpwards2_%s\"));\n"
		"	AttackDown2Animation = LoadObject<UPaperFlipbook>(nullptr, TEXT(\"/Game/Animations/AttackDownwards2_%s\"));\n"
		"	AttackSide2Animation = LoadObject<UPaperFlipbook>(nullptr, TEXT(\"/Game/Animations/AttackSideways2_%s\"));\n"
		"	\n"
		"	// Set initial animation\n"
		"	if (GetSprite() && IdleAnimation)\n"
		"	{\n"
		"		GetSprite()->SetFlipbook(IdleAnimation);\n"
		"		UE_LOG(LogCharacterCreation, Warning, TEXT(\"✓ Set initial animation for %s\"));\n"
		"	}\n"
		"	else\n"
		"	{\n"
		"		UE_LOG(LogCharacterCreation, Warning, TEXT(\"Failed to set animation - Sprite: %%s, IdleAnimation: %%s\"),\n"
		"			GetSprite() ? TEXT(\"Valid\") : TEXT(\"NULL\"),\n"
		"			IdleAnimation ? TEXT(\"Valid\") : TEXT(\"NULL\"));\n"
		"	}\n"
		"}\n"
	), *TextureName, *CharacterName, *CharacterName, *CharacterName, *CharacterName, *CharacterName, *TextureName, 
	   *TextureName, *TextureName, *TextureName, *TextureName, *TextureName, *TextureName, *TextureName, *TextureName, *CharacterName);
	
	if (FFileHelper::SaveStringToFile(SourceContent, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		UE_LOG(LogCharacterCreation, Log, TEXT("Created source file: %s"), *FilePath);
		return true;
	}
	else
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Failed to create source file: %s"), *FilePath);
		return false;
	}
}

void UCharacterCreationCommandlet::PrintBatchSummary(const TArray<FString>& ProcessedTextures, const TArray<FString>& GeneratedCharacters) const
{
	UE_LOG(LogCharacterCreation, Warning, TEXT(""));
	UE_LOG(LogCharacterCreation, Warning, TEXT("════════════════════════════════════════════════════"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("                BATCH PROCESSING SUMMARY             "));
	UE_LOG(LogCharacterCreation, Warning, TEXT("════════════════════════════════════════════════════"));
	UE_LOG(LogCharacterCreation, Warning, TEXT(""));
	
	UE_LOG(LogCharacterCreation, Warning, TEXT("Processed Textures (%d):"), ProcessedTextures.Num());
	for (const FString& Texture : ProcessedTextures)
	{
		UE_LOG(LogCharacterCreation, Warning, TEXT("  ✓ %s"), *Texture);
	}
	
	if (GeneratedCharacters.Num() > 0)
	{
		UE_LOG(LogCharacterCreation, Warning, TEXT(""));
		UE_LOG(LogCharacterCreation, Warning, TEXT("Generated Character Classes (%d):"), GeneratedCharacters.Num());
		for (const FString& Character : GeneratedCharacters)
		{
			UE_LOG(LogCharacterCreation, Warning, TEXT("  ✓ %s"), *Character);
		}
		
		UE_LOG(LogCharacterCreation, Warning, TEXT(""));
		UE_LOG(LogCharacterCreation, Warning, TEXT("IMPORTANT: Run 'make CharacterCreationCppEditor' to compile new classes"));
	}
	
	UE_LOG(LogCharacterCreation, Warning, TEXT(""));
	UE_LOG(LogCharacterCreation, Warning, TEXT("════════════════════════════════════════════════════"));
}