#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "SpriteSheetProcessor.h"

class FSpriteProcessorCommand
{
public:
	static void RegisterCommands();
	static void ProcessSpriteSheet(const TArray<FString>& Args);
};