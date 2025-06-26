#include "SpriteSheetTest.h"

ASpriteSheetTest::ASpriteSheetTest()
{
	PrimaryActorTick.bCanEverTick = false;
	Processor = CreateDefaultSubobject<USpriteSheetProcessor>(TEXT("Processor"));
}

void ASpriteSheetTest::BeginPlay()
{
	Super::BeginPlay();
}

void ASpriteSheetTest::RunSpriteSheetTest()
{
	if (!Processor)
	{
		UE_LOG(LogTemp, Error, TEXT("Processor is null"));
		return;
	}

	FSpriteSheetInfo Info;
	Info.Columns = 6;
	Info.Rows = 8;

	UE_LOG(LogTemp, Warning, TEXT("Starting sprite sheet processing test..."));
	
	bool bSuccess = Processor->ProcessSpriteSheet(TEXT("Warrior_Blue"), Info);
	
	if (bSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("✓ Sprite sheet processing completed successfully!"));
		UE_LOG(LogTemp, Warning, TEXT("Generated 48 sprites (6x8 grid) and 8 animations"));
		UE_LOG(LogTemp, Warning, TEXT("Check Content Browser: /Game/Sprites/ and /Game/Animations/"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("✗ Sprite sheet processing failed"));
	}
}