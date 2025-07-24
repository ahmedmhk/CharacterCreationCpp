// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharacterCreationCpp.h"
#include "Modules/ModuleManager.h"
#include "CharacterCreationLog.h"
#include "CharacterCreationCommandlet.h"

class FCharacterCreationCppModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override
	{
		UE_LOG(LogCharacterCreation, Warning, TEXT("CharacterCreationCpp module started"));
		UE_LOG(LogCharacterCreation, Warning, TEXT("CharacterCreationCommandlet available for command-line execution"));
	}

	virtual void ShutdownModule() override
	{
		UE_LOG(LogCharacterCreation, Warning, TEXT("CharacterCreationCpp module shut down"));
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE( FCharacterCreationCppModule, CharacterCreationCpp, "CharacterCreationCpp" );
