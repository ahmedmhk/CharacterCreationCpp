#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Texture2D.h"
#include "PaperSprite.h"
#include "PaperFlipbook.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "CharacterCreationLog.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "SpriteSheetProcessor.generated.h"

UENUM(BlueprintType)
enum class EAnimationType : uint8
{
	Idle = 0,
	Move = 1,
	AttackSideways = 2,
	AttackSideways2 = 3,
	AttackDownwards = 4,
	AttackDownwards2 = 5,
	AttackUpwards = 6,
	AttackUpwards2 = 7
};

USTRUCT(BlueprintType)
struct FSpriteSheetInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Columns = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Rows = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SourceTexturePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DestinationPath;

	FSpriteSheetInfo()
	{
		Columns = 6;
		Rows = 8;
		SourceTexturePath = TEXT("/Game/RawAssets/");
		DestinationPath = TEXT("/Game/Sprites/");
	}
};

UCLASS(BlueprintType, Blueprintable)
class CHARACTERCREATIONCPP_API USpriteSheetProcessor : public UObject
{
	GENERATED_BODY()

public:
	USpriteSheetProcessor();

	UFUNCTION(BlueprintCallable, Category = "Sprite Processing")
	bool ProcessSpriteSheet(const FString& TextureName, const FSpriteSheetInfo& SpriteInfo);

	UFUNCTION(BlueprintCallable, Category = "Sprite Processing")
	UTexture2D* ImportTexture(const FString& RawAssetPath, const FString& DestinationPath);

	UFUNCTION(BlueprintCallable, Category = "Sprite Processing")
	bool ApplyPaper2DTextureSettings(UTexture2D* Texture);

	UFUNCTION(BlueprintCallable, Category = "Sprite Processing")
	TArray<UPaperSprite*> ExtractSprites(UTexture2D* Texture, const FSpriteSheetInfo& SpriteInfo);

	UFUNCTION(BlueprintCallable, Category = "Sprite Processing")
	TArray<UPaperFlipbook*> CreateAnimations(const TArray<UPaperSprite*>& Sprites, const FSpriteSheetInfo& SpriteInfo, const FString& CharacterName = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "Input Creation")
	UInputAction* CreateInputAction(const FString& ActionName, const FString& PackagePath);

	UFUNCTION(BlueprintCallable, Category = "Input Creation")
	UInputMappingContext* CreateInputMappingContext(const FString& ContextName, const FString& PackagePath, UInputAction* MoveAction, UInputAction* AttackAction);

private:
	FString GetAnimationName(EAnimationType AnimType, const FString& CharacterName = TEXT("")) const;
	UTexture2D* CreateSpriteTexture(uint8* SourceData, int32 SourceWidth, int32 SourceHeight, 
		int32 StartX, int32 StartY, int32 SpriteWidth, int32 SpriteHeight, int32 BytesPerPixel, const FString& SpriteName);
	
	UPROPERTY(Transient, VisibleAnywhere, Category = "Generated Assets")
	TArray<UPaperSprite*> GeneratedSprites;

	UPROPERTY(Transient, VisibleAnywhere, Category = "Generated Assets")
	TArray<UPaperFlipbook*> GeneratedFlipbooks;
};