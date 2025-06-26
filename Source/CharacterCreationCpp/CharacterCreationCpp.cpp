// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharacterCreationCpp.h"
#include "Modules/ModuleManager.h"
#include "CharacterCreationLog.h"
#include "SpriteProcessorCommand.h"
#include "HAL/IConsoleManager.h"

class FCharacterCreationCppModule : public FDefaultGameModuleImpl
{
private:
	IConsoleCommand* ProcessSpriteSheetCmd;

public:
	virtual void StartupModule() override
	{
		UE_LOG(LogCharacterCreation, Warning, TEXT("CharacterCreationCpp module started"));
		
		// Register console command explicitly in module startup
		ProcessSpriteSheetCmd = IConsoleManager::Get().RegisterConsoleCommand(
			TEXT("ProcessSpriteSheet"),
			TEXT("Process the Warrior_Blue sprite sheet. Usage: ProcessSpriteSheet"),
			FConsoleCommandWithArgsDelegate::CreateStatic(FSpriteProcessorCommand::ProcessSpriteSheet),
			ECVF_Default
		);
		
		UE_LOG(LogCharacterCreation, Warning, TEXT("ProcessSpriteSheet console command registered"));
	}

	virtual void ShutdownModule() override
	{
		UE_LOG(LogCharacterCreation, Warning, TEXT("CharacterCreationCpp module shut down"));
		
		// Unregister console command
		if (ProcessSpriteSheetCmd)
		{
			IConsoleManager::Get().UnregisterConsoleObject(ProcessSpriteSheetCmd);
			ProcessSpriteSheetCmd = nullptr;
		}
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE( FCharacterCreationCppModule, CharacterCreationCpp, "CharacterCreationCpp" );
