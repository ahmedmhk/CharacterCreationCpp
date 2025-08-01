#include "CharacterCreationCommandlet.h"
#include "CharacterCreationLog.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Misc/PackageName.h"
#include "Engine/Engine.h"

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
	bool bDryRun = FParse::Param(*Params, TEXT("dryrun"));

	// Parse optional parameters with defaults
	FSpriteSheetInfo SpriteInfo;
	int32 Columns = DefaultColumns, Rows = DefaultRows;
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

	// Validate grid dimensions
	if (Columns <= 0 || Columns > MaxGridDimension || Rows <= 0 || Rows > MaxGridDimension)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Invalid grid dimensions: columns=%d, rows=%d (valid range: 1-%d)"), Columns, Rows, MaxGridDimension);
		return 1;
	}

	// Validate paths
	if (!ValidateAndSanitizePath(SourcePath) || !ValidateAndSanitizePath(DestPath))
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Invalid source or destination path"));
		return 1;
	}

	SpriteInfo.Columns = Columns;
	SpriteInfo.Rows = Rows;
	SpriteInfo.SourceTexturePath = SourcePath;
	SpriteInfo.DestinationPath = DestPath;

	if (bBatchMode)
	{
		UE_LOG(LogCharacterCreation, Warning, TEXT("Running in BATCH MODE"));
		UE_LOG(LogCharacterCreation, Warning, TEXT("Create Characters: %s"), bCreateCharacters ? TEXT("YES") : TEXT("NO"));
		UE_LOG(LogCharacterCreation, Warning, TEXT("Dry Run: %s"), bDryRun ? TEXT("YES") : TEXT("NO"));
		
		// Validate asset references if creating characters
		if (bCreateCharacters && !bDryRun && !ValidateAssetReferences())
		{
			UE_LOG(LogCharacterCreation, Error, TEXT("Asset reference validation failed"));
			return 1;
		}
		
		bool bSuccess = BatchProcessSpriteSheets(SpriteInfo, bCreateCharacters, bDryRun);
		
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
		UE_LOG(LogCharacterCreation, Warning, TEXT("Dry Run: %s"), bDryRun ? TEXT("YES") : TEXT("NO"));

		// Validate asset references if creating characters
		if (bCreateCharacters && !bDryRun && !ValidateAssetReferences())
		{
			UE_LOG(LogCharacterCreation, Error, TEXT("Asset reference validation failed"));
			return 1;
		}

		if (bDryRun)
		{
			TArray<FString> FilesToProcess;
			FilesToProcess.Add(TextureName);
			PrintDryRunSummary(FilesToProcess);
			UE_LOG(LogCharacterCreation, Warning, TEXT("=== Character Creation Commandlet Dry Run Completed ==="));
			return 0;
		}

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
	USpriteSheetProcessor* Processor = NewObject<USpriteSheetProcessor>(GetTransientPackage());
	
	if (!Processor)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("CRITICAL ERROR: Failed to create USpriteSheetProcessor instance"));
		return false;
	}
	
	UE_LOG(LogCharacterCreation, Warning, TEXT("USpriteSheetProcessor instance created successfully"));

	UE_LOG(LogCharacterCreation, Warning, TEXT("About to call ProcessSpriteSheet(\"%s\", SpriteInfo)..."), *TextureName);
	
	bool bSuccess = false;
	UE_LOG(LogCharacterCreation, Warning, TEXT("CALLING Processor->ProcessSpriteSheet now..."));
	bSuccess = Processor->ProcessSpriteSheet(TextureName, SpriteInfo);
	UE_LOG(LogCharacterCreation, Warning, TEXT("ProcessSpriteSheet call returned: %s"), bSuccess ? TEXT("TRUE") : TEXT("FALSE"));

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
	UE_LOG(LogCharacterCreation, Warning, TEXT("  -dryrun                          Preview operations without executing"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  -columns=<num>   | -c=<num>      Number of columns (default: %d)"), DefaultColumns);
	UE_LOG(LogCharacterCreation, Warning, TEXT("  -rows=<num>      | -r=<num>      Number of rows (default: %d)"), DefaultRows);
	UE_LOG(LogCharacterCreation, Warning, TEXT("  -source=<path>   | -s=<path>     Source texture path (default: /Game/RawAssets/)"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  -dest=<path>     | -d=<path>     Destination path (default: /Game/Sprites/)"));
	UE_LOG(LogCharacterCreation, Warning, TEXT(""));
	UE_LOG(LogCharacterCreation, Warning, TEXT("Examples:"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  CharacterCreationCommandlet -texture=Warrior_Blue"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  CharacterCreationCommandlet -batch -createcharacter"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  CharacterCreationCommandlet -t=Warrior_Blue -c=6 -r=8 -createcharacter"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  CharacterCreationCommandlet -batch -dryrun"));
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

bool UCharacterCreationCommandlet::BatchProcessSpriteSheets(const FSpriteSheetInfo& SpriteInfo, bool bCreateCharacters, bool bDryRun)
{
	UE_LOG(LogCharacterCreation, Warning, TEXT(""));
	UE_LOG(LogCharacterCreation, Warning, TEXT("=== Starting Batch Processing ==="));
	
	// Get the RawAssets directory path
	FString RawAssetsPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("RawAssets/"));
	
	// Validate the path is within project directory
	if (!RawAssetsPath.StartsWith(FPaths::ProjectDir()))
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Invalid RawAssets path: %s"), *RawAssetsPath);
		return false;
	}
	
	// Find all PNG files in the directory
	TArray<FString> PNGFiles;
	IFileManager::Get().FindFiles(PNGFiles, *(RawAssetsPath + TEXT("*.png")), true, false);
	
	if (PNGFiles.Num() == 0)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("No PNG files found in RawAssets directory: %s"), *RawAssetsPath);
		return false;
	}
	
	UE_LOG(LogCharacterCreation, Warning, TEXT("Found %d PNG files to process"), PNGFiles.Num());
	
	// Handle dry run mode
	if (bDryRun)
	{
		TArray<FString> TextureNames;
		for (const FString& PNGFile : PNGFiles)
		{
			TextureNames.Add(FPaths::GetBaseFilename(PNGFile));
		}
		PrintDryRunSummary(TextureNames);
		return true;
	}
	
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
	
	// Clean up memory after batch processing
	CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
	
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
	
	TStringBuilder<2048> SourceContent;
	SourceContent.Appendf(TEXT("// Generated character class for %s\n"), *TextureName);
	SourceContent.Appendf(TEXT("#include \"%s.h\"\n"), *CharacterName);
	SourceContent.Append(TEXT("#include \"PaperFlipbook.h\"\n"));
	SourceContent.Append(TEXT("#include \"PaperFlipbookComponent.h\"\n"));
	SourceContent.Append(TEXT("#include \"CharacterCreationLog.h\"\n"));
	SourceContent.Append(TEXT("\n"));
	SourceContent.Appendf(TEXT("A%s::A%s()\n"), *CharacterName, *CharacterName);
	SourceContent.Append(TEXT("{\n"));
	SourceContent.Append(TEXT("	// Character is already set up by parent class\n"));
	SourceContent.Append(TEXT("	// Load animations in constructor for editor preview\n"));
	SourceContent.Append(TEXT("	LoadAnimationsForCharacter();\n"));
	SourceContent.Append(TEXT("}\n"));
	SourceContent.Append(TEXT("\n"));
	SourceContent.Appendf(TEXT("void A%s::LoadAnimationsForCharacter()\n"), *CharacterName);
	SourceContent.Append(TEXT("{\n"));
	SourceContent.Appendf(TEXT("	UE_LOG(LogCharacterCreation, Warning, TEXT(\"Loading animations for %s\"));\n"), *CharacterName);
	SourceContent.Append(TEXT("	\n"));
	SourceContent.Appendf(TEXT("	// Load animations specific to %s\n"), *TextureName);
	SourceContent.Appendf(TEXT("	IdleAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT(\"/Game/Animations/Idle_%s\"));\n"), *TextureName);
	SourceContent.Appendf(TEXT("	MoveAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT(\"/Game/Animations/Move_%s\"));\n"), *TextureName);
	SourceContent.Appendf(TEXT("	AttackUpAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT(\"/Game/Animations/AttackUpwards_%s\"));\n"), *TextureName);
	SourceContent.Appendf(TEXT("	AttackDownAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT(\"/Game/Animations/AttackDownwards_%s\"));\n"), *TextureName);
	SourceContent.Appendf(TEXT("	AttackSideAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT(\"/Game/Animations/AttackSideways_%s\"));\n"), *TextureName);
	SourceContent.Appendf(TEXT("	AttackUp2Animation = LoadObject<UPaperFlipbook>(nullptr, TEXT(\"/Game/Animations/AttackUpwards2_%s\"));\n"), *TextureName);
	SourceContent.Appendf(TEXT("	AttackDown2Animation = LoadObject<UPaperFlipbook>(nullptr, TEXT(\"/Game/Animations/AttackDownwards2_%s\"));\n"), *TextureName);
	SourceContent.Appendf(TEXT("	AttackSide2Animation = LoadObject<UPaperFlipbook>(nullptr, TEXT(\"/Game/Animations/AttackSideways2_%s\"));\n"), *TextureName);
	SourceContent.Append(TEXT("	\n"));
	SourceContent.Append(TEXT("	// Set initial animation\n"));
	SourceContent.Append(TEXT("	if (GetSprite() && IdleAnimation)\n"));
	SourceContent.Append(TEXT("	{\n"));
	SourceContent.Append(TEXT("		GetSprite()->SetFlipbook(IdleAnimation);\n"));
	SourceContent.Appendf(TEXT("		UE_LOG(LogCharacterCreation, Warning, TEXT(\"✓ Set initial animation for %s\"));\n"), *CharacterName);
	SourceContent.Append(TEXT("	}\n"));
	SourceContent.Append(TEXT("	else\n"));
	SourceContent.Append(TEXT("	{\n"));
	SourceContent.Append(TEXT("		UE_LOG(LogCharacterCreation, Warning, TEXT(\"Failed to set animation - Sprite: %s, IdleAnimation: %s\"),\n"));
	SourceContent.Append(TEXT("			GetSprite() ? TEXT(\"Valid\") : TEXT(\"NULL\"),\n"));
	SourceContent.Append(TEXT("			IdleAnimation ? TEXT(\"Valid\") : TEXT(\"NULL\"));\n"));
	SourceContent.Append(TEXT("	}\n"));
	SourceContent.Append(TEXT("}\n"));
	
	if (FFileHelper::SaveStringToFile(SourceContent.ToString(), *FilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
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

bool UCharacterCreationCommandlet::ValidateAndSanitizePath(FString& Path) const
{
	// Ensure path starts with /Game/
	if (!Path.StartsWith(TEXT("/Game/")))
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Path must start with /Game/: %s"), *Path);
		return false;
	}
	
	// Check for directory traversal attempts
	if (Path.Contains(TEXT("..")) || Path.Contains(TEXT("//")))
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Invalid path characters detected: %s"), *Path);
		return false;
	}
	
	// Ensure path ends with slash
	if (!Path.EndsWith(TEXT("/")))
	{
		Path += TEXT("/");
	}
	
	// Validate it's a valid package path
	if (!FPackageName::IsValidLongPackageName(Path.LeftChop(1))) // Remove trailing slash for validation
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Invalid package path: %s"), *Path);
		return false;
	}
	
	return true;
}

bool UCharacterCreationCommandlet::ValidateAssetReferences() const
{
	// Check if WarriorCharacter class exists
	UClass* WarriorClass = FindObject<UClass>(ANY_PACKAGE, TEXT("AWarriorCharacter"));
	if (!WarriorClass)
	{
		// Try loading it
		WarriorClass = LoadObject<UClass>(nullptr, TEXT("/Script/CharacterCreationCpp.WarriorCharacter"));
		if (!WarriorClass)
		{
			UE_LOG(LogCharacterCreation, Error, TEXT("Cannot find AWarriorCharacter class - ensure it exists and is compiled"));
			UE_LOG(LogCharacterCreation, Error, TEXT("Run 'make CharacterCreationCppEditor' to compile the project first"));
			return false;
		}
	}
	
	UE_LOG(LogCharacterCreation, Log, TEXT("✓ Found AWarriorCharacter base class"));
	return true;
}

void UCharacterCreationCommandlet::PrintDryRunSummary(const TArray<FString>& FilesToProcess) const
{
	UE_LOG(LogCharacterCreation, Warning, TEXT(""));
	UE_LOG(LogCharacterCreation, Warning, TEXT("════════════════════════════════════════════════════"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("                  DRY RUN SUMMARY                    "));
	UE_LOG(LogCharacterCreation, Warning, TEXT("════════════════════════════════════════════════════"));
	UE_LOG(LogCharacterCreation, Warning, TEXT(""));
	UE_LOG(LogCharacterCreation, Warning, TEXT("The following operations would be performed:"));
	UE_LOG(LogCharacterCreation, Warning, TEXT(""));
	
	UE_LOG(LogCharacterCreation, Warning, TEXT("Textures to Process (%d):"), FilesToProcess.Num());
	for (const FString& Texture : FilesToProcess)
	{
		UE_LOG(LogCharacterCreation, Warning, TEXT("  • %s.png"), *Texture);
		UE_LOG(LogCharacterCreation, Warning, TEXT("    → Generate 48 sprites"));
		UE_LOG(LogCharacterCreation, Warning, TEXT("    → Create 8 animations"));
	}
	
	UE_LOG(LogCharacterCreation, Warning, TEXT(""));
	UE_LOG(LogCharacterCreation, Warning, TEXT("Total Assets to Generate:"));
	UE_LOG(LogCharacterCreation, Warning, TEXT("  • %d Sprites"), FilesToProcess.Num() * 48);
	UE_LOG(LogCharacterCreation, Warning, TEXT("  • %d Animations"), FilesToProcess.Num() * 8);
	
	UE_LOG(LogCharacterCreation, Warning, TEXT(""));
	UE_LOG(LogCharacterCreation, Warning, TEXT("No files have been created or modified."));
	UE_LOG(LogCharacterCreation, Warning, TEXT("Remove -dryrun flag to execute these operations."));
	UE_LOG(LogCharacterCreation, Warning, TEXT("════════════════════════════════════════════════════"));
}