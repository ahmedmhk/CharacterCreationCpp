#include "SpriteSheetProcessorExample.h"
#include "Engine/Engine.h"

ASpriteSheetProcessorExample::ASpriteSheetProcessorExample()
{
	PrimaryActorTick.bCanEverTick = false;

	SpriteProcessor = CreateDefaultSubobject<USpriteSheetProcessor>(TEXT("SpriteProcessor"));
	
	SpriteSheetSettings.Columns = 6;
	SpriteSheetSettings.Rows = 8;
	SpriteSheetSettings.SourceTexturePath = TEXT("/Game/RawAssets/");
	SpriteSheetSettings.DestinationPath = TEXT("/Game/Sprites/");
}

void ASpriteSheetProcessorExample::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Log, TEXT("SpriteSheetProcessorExample: Ready to process sprite sheets"));
	UE_LOG(LogTemp, Log, TEXT("Call ProcessWarriorSpriteSheet() to process the %s sprite sheet"), *TextureFileName);
}

void ASpriteSheetProcessorExample::ProcessWarriorSpriteSheet()
{
	if (!SpriteProcessor)
	{
		UE_LOG(LogTemp, Error, TEXT("SpriteProcessor is null!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Starting sprite sheet processing for: %s"), *TextureFileName);
	UE_LOG(LogTemp, Log, TEXT("Grid dimensions: %d columns x %d rows"), SpriteSheetSettings.Columns, SpriteSheetSettings.Rows);
	
	bool bSuccess = SpriteProcessor->ProcessSpriteSheet(TextureFileName, SpriteSheetSettings);
	
	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully processed sprite sheet!"));
		UE_LOG(LogTemp, Log, TEXT("Generated sprites and animations:"));
		UE_LOG(LogTemp, Log, TEXT("- Row 1: Idle (6 frames)"));
		UE_LOG(LogTemp, Log, TEXT("- Row 2: Move (6 frames)"));
		UE_LOG(LogTemp, Log, TEXT("- Row 3: AttackSideways (6 frames)"));
		UE_LOG(LogTemp, Log, TEXT("- Row 4: AttackSideways2 (6 frames)"));
		UE_LOG(LogTemp, Log, TEXT("- Row 5: AttackDownwards (6 frames)"));
		UE_LOG(LogTemp, Log, TEXT("- Row 6: AttackDownwards2 (6 frames)"));
		UE_LOG(LogTemp, Log, TEXT("- Row 7: AttackUpwards (6 frames)"));
		UE_LOG(LogTemp, Log, TEXT("- Row 8: AttackUpwards2 (6 frames)"));
		UE_LOG(LogTemp, Log, TEXT("Check /Game/Sprites/ for individual sprites"));
		UE_LOG(LogTemp, Log, TEXT("Check /Game/Animations/ for flipbook animations"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to process sprite sheet!"));
	}
}

void ASpriteSheetProcessorExample::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}