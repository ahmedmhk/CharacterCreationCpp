// Copyright Epic Games, Inc. All Rights Reserved.

#include "PaperFlipbookProject.h"
#include "Modules/ModuleManager.h"
#include "SpriteProcessorCommand.h"

class FPaperFlipbookProjectModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override
	{
		FSpriteProcessorCommand::RegisterCommands();
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE( FPaperFlipbookProjectModule, PaperFlipbookProject, "PaperFlipbookProject" );
