#include "GameModeCreationCommandlet.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"

DEFINE_LOG_CATEGORY(LogGameModeCreation);

UGameModeCreationCommandlet::UGameModeCreationCommandlet()
{
    IsClient = false;
    IsEditor = true;
    IsServer = false;
    LogToConsole = true;
}

int32 UGameModeCreationCommandlet::Main(const FString& Params)
{
    UE_LOG(LogGameModeCreation, Warning, TEXT("=== Game Mode Creation Commandlet Started ==="));
    UE_LOG(LogGameModeCreation, Warning, TEXT("Creating MyGameMode with WarriorPurpleCharacter as default pawn..."));
    
    bool bSuccess = true;
    
    // Step 1: Create the game mode class files
    if (!CreateGameModeClass())
    {
        UE_LOG(LogGameModeCreation, Error, TEXT("Failed to create game mode class files"));
        bSuccess = false;
    }
    else
    {
        UE_LOG(LogGameModeCreation, Warning, TEXT("✓ Successfully created MyGameMode class files"));
    }
    
    // Step 2: Update DefaultEngine.ini to set the default game mode
    if (bSuccess && !UpdateDefaultEngineIni())
    {
        UE_LOG(LogGameModeCreation, Error, TEXT("Failed to update DefaultEngine.ini"));
        bSuccess = false;
    }
    else if (bSuccess)
    {
        UE_LOG(LogGameModeCreation, Warning, TEXT("✓ Successfully updated DefaultEngine.ini"));
    }
    
    if (bSuccess)
    {
        UE_LOG(LogGameModeCreation, Warning, TEXT(""));
        UE_LOG(LogGameModeCreation, Warning, TEXT("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"));
        UE_LOG(LogGameModeCreation, Warning, TEXT("✓ SUCCESS! Game Mode Creation Complete"));
        UE_LOG(LogGameModeCreation, Warning, TEXT("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"));
        UE_LOG(LogGameModeCreation, Warning, TEXT(""));
        UE_LOG(LogGameModeCreation, Warning, TEXT("Created Files:"));
        UE_LOG(LogGameModeCreation, Warning, TEXT("  • Source/CharacterCreationCpp/MyGameMode.h"));
        UE_LOG(LogGameModeCreation, Warning, TEXT("  • Source/CharacterCreationCpp/MyGameMode.cpp"));
        UE_LOG(LogGameModeCreation, Warning, TEXT(""));
        UE_LOG(LogGameModeCreation, Warning, TEXT("Configuration:"));
        UE_LOG(LogGameModeCreation, Warning, TEXT("  • Default Pawn Class: WarriorPurpleCharacter"));
        UE_LOG(LogGameModeCreation, Warning, TEXT("  • Default Game Mode: MyGameMode"));
        UE_LOG(LogGameModeCreation, Warning, TEXT(""));
        UE_LOG(LogGameModeCreation, Warning, TEXT("⚠️  IMPORTANT: Recompile the project to use the new game mode!"));
        UE_LOG(LogGameModeCreation, Warning, TEXT(""));
    }
    else
    {
        UE_LOG(LogGameModeCreation, Error, TEXT("✗ Game Mode creation failed!"));
    }
    
    return bSuccess ? 0 : 1;
}

bool UGameModeCreationCommandlet::CreateGameModeClass()
{
    // Get the source directory path
    FString ProjectDir = FPaths::ProjectDir();
    FString SourceDir = FPaths::Combine(ProjectDir, TEXT("Source"), TEXT("CharacterCreationCpp"));
    
    // Create file paths
    FString HeaderPath = FPaths::Combine(SourceDir, TEXT("MyGameMode.h"));
    FString CppPath = FPaths::Combine(SourceDir, TEXT("MyGameMode.cpp"));
    
    // Generate file contents
    FString HeaderContent = GenerateGameModeHeader();
    FString CppContent = GenerateGameModeCpp();
    
    // Write header file
    if (!WriteSourceFile(HeaderPath, HeaderContent))
    {
        UE_LOG(LogGameModeCreation, Error, TEXT("Failed to write header file: %s"), *HeaderPath);
        return false;
    }
    UE_LOG(LogGameModeCreation, Warning, TEXT("Created header file: %s"), *HeaderPath);
    
    // Write cpp file
    if (!WriteSourceFile(CppPath, CppContent))
    {
        UE_LOG(LogGameModeCreation, Error, TEXT("Failed to write cpp file: %s"), *CppPath);
        return false;
    }
    UE_LOG(LogGameModeCreation, Warning, TEXT("Created cpp file: %s"), *CppPath);
    
    return true;
}

FString UGameModeCreationCommandlet::GenerateGameModeHeader()
{
    return TEXT("#pragma once\n")
           TEXT("\n")
           TEXT("#include \"CoreMinimal.h\"\n")
           TEXT("#include \"GameFramework/GameModeBase.h\"\n")
           TEXT("#include \"MyGameMode.generated.h\"\n")
           TEXT("\n")
           TEXT("/**\n")
           TEXT(" * Custom game mode that uses WarriorPurpleCharacter as the default pawn\n")
           TEXT(" */\n")
           TEXT("UCLASS()\n")
           TEXT("class CHARACTERCREATIONCPP_API AMyGameMode : public AGameModeBase\n")
           TEXT("{\n")
           TEXT("    GENERATED_BODY()\n")
           TEXT("\n")
           TEXT("public:\n")
           TEXT("    AMyGameMode();\n")
           TEXT("\n")
           TEXT("    /** Called when the game starts or when spawned */\n")
           TEXT("    virtual void BeginPlay() override;\n")
           TEXT("\n")
           TEXT("protected:\n")
           TEXT("    /** Override to specify the default pawn class */\n")
           TEXT("    virtual void InitGameState() override;\n")
           TEXT("};\n");
}

FString UGameModeCreationCommandlet::GenerateGameModeCpp()
{
    return TEXT("#include \"MyGameMode.h\"\n")
           TEXT("#include \"CharacterCreationCommandlet/WarriorPurpleCharacter.h\"\n")
           TEXT("#include \"Engine/Engine.h\"\n")
           TEXT("#include \"Kismet/GameplayStatics.h\"\n")
           TEXT("\n")
           TEXT("AMyGameMode::AMyGameMode()\n")
           TEXT("{\n")
           TEXT("    // Set WarriorPurpleCharacter as the default pawn class\n")
           TEXT("    static ConstructorHelpers::FClassFinder<APawn> PawnClassFinder(TEXT(\"/Script/CharacterCreationCpp.WarriorPurpleCharacter\"));\n")
           TEXT("    if (PawnClassFinder.Succeeded())\n")
           TEXT("    {\n")
           TEXT("        DefaultPawnClass = PawnClassFinder.Class;\n")
           TEXT("        UE_LOG(LogTemp, Warning, TEXT(\"MyGameMode: Successfully set WarriorPurpleCharacter as default pawn\"));\n")
           TEXT("    }\n")
           TEXT("    else\n")
           TEXT("    {\n")
           TEXT("        // Fallback: Try to load the class dynamically\n")
           TEXT("        DefaultPawnClass = AWarriorPurpleCharacter::StaticClass();\n")
           TEXT("        UE_LOG(LogTemp, Warning, TEXT(\"MyGameMode: Set WarriorPurpleCharacter using StaticClass()\"));\n")
           TEXT("    }\n")
           TEXT("    \n")
           TEXT("    // Log the default pawn class for debugging\n")
           TEXT("    if (DefaultPawnClass)\n")
           TEXT("    {\n")
           TEXT("        UE_LOG(LogTemp, Warning, TEXT(\"MyGameMode: Default Pawn Class is %s\"), *DefaultPawnClass->GetName());\n")
           TEXT("    }\n")
           TEXT("}\n")
           TEXT("\n")
           TEXT("void AMyGameMode::BeginPlay()\n")
           TEXT("{\n")
           TEXT("    Super::BeginPlay();\n")
           TEXT("    \n")
           TEXT("    UE_LOG(LogTemp, Warning, TEXT(\"MyGameMode: Game mode started with WarriorPurpleCharacter as default pawn\"));\n")
           TEXT("    \n")
           TEXT("    // Verify the default pawn class is set\n")
           TEXT("    if (DefaultPawnClass)\n")
           TEXT("    {\n")
           TEXT("        UE_LOG(LogTemp, Warning, TEXT(\"MyGameMode: Using pawn class: %s\"), *DefaultPawnClass->GetName());\n")
           TEXT("    }\n")
           TEXT("    else\n")
           TEXT("    {\n")
           TEXT("        UE_LOG(LogTemp, Error, TEXT(\"MyGameMode: No default pawn class set!\"));\n")
           TEXT("    }\n")
           TEXT("}\n")
           TEXT("\n")
           TEXT("void AMyGameMode::InitGameState()\n")
           TEXT("{\n")
           TEXT("    Super::InitGameState();\n")
           TEXT("    \n")
           TEXT("    // Additional initialization if needed\n")
           TEXT("    UE_LOG(LogTemp, Warning, TEXT(\"MyGameMode: Game state initialized\"));\n")
           TEXT("}\n");
}

bool UGameModeCreationCommandlet::WriteSourceFile(const FString& FilePath, const FString& Content)
{
    // Ensure the directory exists
    FString Directory = FPaths::GetPath(FilePath);
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.DirectoryExists(*Directory))
    {
        if (!PlatformFile.CreateDirectoryTree(*Directory))
        {
            UE_LOG(LogGameModeCreation, Error, TEXT("Failed to create directory: %s"), *Directory);
            return false;
        }
    }
    
    // Write the file
    if (!FFileHelper::SaveStringToFile(Content, *FilePath))
    {
        UE_LOG(LogGameModeCreation, Error, TEXT("Failed to write file: %s"), *FilePath);
        return false;
    }
    
    return true;
}

bool UGameModeCreationCommandlet::UpdateDefaultEngineIni()
{
    // Get the config file path
    FString ConfigPath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Config"), TEXT("DefaultEngine.ini"));
    
    // Read the existing config
    FString ConfigContent;
    if (!FFileHelper::LoadFileToString(ConfigContent, *ConfigPath))
    {
        UE_LOG(LogGameModeCreation, Error, TEXT("Failed to read DefaultEngine.ini"));
        return false;
    }
    
    // Check if [/Script/EngineSettings.GameMapsSettings] section exists
    FString GameMapsSection = TEXT("[/Script/EngineSettings.GameMapsSettings]");
    int32 SectionIndex = ConfigContent.Find(GameMapsSection);
    
    FString GameModeEntry = TEXT("GlobalDefaultGameMode=/Script/CharacterCreationCpp.MyGameMode");
    
    if (SectionIndex != INDEX_NONE)
    {
        // Section exists, find where to insert the game mode setting
        int32 NextSectionIndex = ConfigContent.Find(TEXT("\n["), SectionIndex + GameMapsSection.Len());
        
        // Check if GlobalDefaultGameMode already exists
        int32 GameModeIndex = ConfigContent.Find(TEXT("GlobalDefaultGameMode="), SectionIndex);
        
        if (GameModeIndex != INDEX_NONE && (NextSectionIndex == INDEX_NONE || GameModeIndex < NextSectionIndex))
        {
            // Replace existing GlobalDefaultGameMode
            int32 LineEnd = ConfigContent.Find(TEXT("\n"), GameModeIndex);
            if (LineEnd == INDEX_NONE)
            {
                LineEnd = ConfigContent.Len();
            }
            
            FString OldLine = ConfigContent.Mid(GameModeIndex, LineEnd - GameModeIndex);
            ConfigContent = ConfigContent.Replace(*OldLine, *GameModeEntry);
            
            UE_LOG(LogGameModeCreation, Warning, TEXT("Replaced existing GlobalDefaultGameMode setting"));
        }
        else
        {
            // Add GlobalDefaultGameMode to the section
            int32 InsertPos = SectionIndex + GameMapsSection.Len();
            ConfigContent = ConfigContent.Left(InsertPos) + TEXT("\n") + GameModeEntry + ConfigContent.Mid(InsertPos);
            
            UE_LOG(LogGameModeCreation, Warning, TEXT("Added GlobalDefaultGameMode setting to existing section"));
        }
    }
    else
    {
        // Section doesn't exist, add it
        FString NewSection = TEXT("\n\n") + GameMapsSection + TEXT("\n") + GameModeEntry;
        ConfigContent += NewSection;
        
        UE_LOG(LogGameModeCreation, Warning, TEXT("Added new GameMapsSettings section with GlobalDefaultGameMode"));
    }
    
    // Write the updated config back
    if (!FFileHelper::SaveStringToFile(ConfigContent, *ConfigPath))
    {
        UE_LOG(LogGameModeCreation, Error, TEXT("Failed to write DefaultEngine.ini"));
        return false;
    }
    
    UE_LOG(LogGameModeCreation, Warning, TEXT("Updated DefaultEngine.ini with default game mode: MyGameMode"));
    return true;
}