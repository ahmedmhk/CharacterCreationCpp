#include "SpriteSheetProcessor.h"
#include "Engine/Texture2D.h"
#include "PaperSprite.h"
#include "PaperFlipbook.h"
#include "Engine/Engine.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/ConstructorHelpers.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "PackageTools.h"
#include "ObjectTools.h"
#include "PaperSpriteComponent.h"
#include "SpriteEditorOnlyTypes.h"
#include "RenderingThread.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "UObject/SavePackage.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"

USpriteSheetProcessor::USpriteSheetProcessor()
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return;
	}
}

bool USpriteSheetProcessor::ProcessSpriteSheet(const FString& TextureName, const FSpriteSheetInfo& SpriteInfo)
{
	FString RawAssetPath = FPaths::ProjectDir() + TEXT("RawAssets/") + TextureName + TEXT(".png");
	FString DestinationPath = TEXT("/Game/") + TextureName;

	UTexture2D* ImportedTexture = ImportTexture(RawAssetPath, DestinationPath);
	if (!ImportedTexture)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Failed to import texture: %s"), *TextureName);
		return false;
	}

	// Log imported texture size (Paper2D settings already applied during creation)
	UE_LOG(LogCharacterCreation, Warning, TEXT("Imported texture size: %dx%d"), ImportedTexture->GetSizeX(), ImportedTexture->GetSizeY());

	// Now safely extract sprites from the properly configured texture
	TArray<UPaperSprite*> ExtractedSprites = ExtractSprites(ImportedTexture, SpriteInfo);
	if (ExtractedSprites.Num() == 0)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Failed to extract sprites from texture: %s"), *TextureName);
		return false;
	}

	TArray<UPaperFlipbook*> CreatedAnimations = CreateAnimations(ExtractedSprites, SpriteInfo);
	if (CreatedAnimations.Num() == 0)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Failed to create animations from sprites"));
		return false;
	}

	GeneratedSprites = ExtractedSprites;
	GeneratedFlipbooks = CreatedAnimations;

	// Create Input Actions
	UE_LOG(LogCharacterCreation, Warning, TEXT("Creating Input Actions..."));
	UInputAction* MoveAction = CreateInputAction(TEXT("IA_Move"), TEXT("/Game/Input/IA_Move"));
	UInputAction* AttackAction = CreateInputAction(TEXT("IA_Attack"), TEXT("/Game/Input/IA_Attack"));

	// Create Input Mapping Context
	UInputMappingContext* MappingContext = nullptr;
	if (MoveAction && AttackAction)
	{
		UE_LOG(LogCharacterCreation, Warning, TEXT("Creating Input Mapping Context..."));
		MappingContext = CreateInputMappingContext(TEXT("IMC_PlayerInput"), TEXT("/Game/Input/IMC_PlayerInput"), MoveAction, AttackAction);
	}

	UE_LOG(LogCharacterCreation, Log, TEXT("Successfully processed sprite sheet: %s"), *TextureName);
	UE_LOG(LogCharacterCreation, Log, TEXT("Generated %d sprites and %d animations"), ExtractedSprites.Num(), CreatedAnimations.Num());
	
	if (MoveAction && AttackAction && MappingContext)
	{
		UE_LOG(LogCharacterCreation, Warning, TEXT("✓ Input system created: IA_Move, IA_Attack, and IMC_PlayerInput"));
	}

	return true;
}

UTexture2D* USpriteSheetProcessor::ImportTexture(const FString& RawAssetPath, const FString& DestinationPath)
{
#if WITH_EDITOR
	if (!FPaths::FileExists(RawAssetPath))
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Source texture file does not exist: %s"), *RawAssetPath);
		return nullptr;
	}

	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *RawAssetPath))
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Failed to load texture file: %s"), *RawAssetPath);
		return nullptr;
	}

	UE_LOG(LogCharacterCreation, Warning, TEXT("Raw file data size: %d bytes"), RawFileData.Num());

	// Use IImageWrapper to decode PNG and get exact dimensions
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	
	if (!ImageWrapper.IsValid())
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Failed to create PNG image wrapper"));
		return nullptr;
	}

	if (!ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Failed to set compressed data in image wrapper"));
		return nullptr;
	}

	int32 Width = ImageWrapper->GetWidth();
	int32 Height = ImageWrapper->GetHeight();
	UE_LOG(LogCharacterCreation, Warning, TEXT("Decoded image dimensions: %dx%d"), Width, Height);

	// Extract raw RGBA pixel data
	TArray<uint8> UncompressedRGBA;
	if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedRGBA))
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Failed to get raw pixel data from image"));
		return nullptr;
	}

	// Create package and texture manually
	FString PackageName = DestinationPath;
	FString AssetName = FPaths::GetBaseFilename(DestinationPath);
	
	UPackage* Package = CreatePackage(*PackageName);
	if (!Package)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Failed to create package: %s"), *PackageName);
		return nullptr;
	}

	// Create texture manually with exact dimensions
	UTexture2D* NewTexture = NewObject<UTexture2D>(Package, *AssetName, RF_Public | RF_Standalone);
	if (!NewTexture)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Failed to create texture object"));
		return nullptr;
	}

	// Apply Paper2D settings BEFORE initializing platform data
	NewTexture->Filter = TF_Nearest;
	NewTexture->MipGenSettings = TMGS_NoMipmaps;
	NewTexture->CompressionSettings = TC_VectorDisplacementmap;
	NewTexture->SRGB = false;
	NewTexture->LODGroup = TEXTUREGROUP_World;
	NewTexture->MaxTextureSize = 0;
	NewTexture->PowerOfTwoMode = ETexturePowerOfTwoSetting::None;
	NewTexture->NeverStream = true;

	// Initialize texture with exact pixel data
	NewTexture->SetPlatformData(new FTexturePlatformData());
	NewTexture->GetPlatformData()->SizeX = Width;
	NewTexture->GetPlatformData()->SizeY = Height;
	NewTexture->GetPlatformData()->PixelFormat = PF_B8G8R8A8;

	// Create mip map with exact size
	FTexture2DMipMap* Mip = new FTexture2DMipMap();
	NewTexture->GetPlatformData()->Mips.Add(Mip);
	Mip->SizeX = Width;
	Mip->SizeY = Height;
	
	// Copy pixel data
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* TextureData = static_cast<uint8*>(Mip->BulkData.Realloc(UncompressedRGBA.Num()));
	FMemory::Memcpy(TextureData, UncompressedRGBA.GetData(), UncompressedRGBA.Num());
	Mip->BulkData.Unlock();

	// Initialize source data
	NewTexture->Source.Init(Width, Height, 1, 1, TSF_BGRA8, UncompressedRGBA.GetData());
	
	FAssetRegistryModule::AssetCreated(NewTexture);
	Package->MarkPackageDirty();
	
	UE_LOG(LogCharacterCreation, Log, TEXT("Successfully imported texture: %s (%dx%d)"), *AssetName, Width, Height);
	return NewTexture;
#else
	UE_LOG(LogCharacterCreation, Error, TEXT("Import functionality is only available in editor builds"));
	return nullptr;
#endif
}

bool USpriteSheetProcessor::ApplyPaper2DTextureSettings(UTexture2D* Texture)
{
	if (!Texture)
	{
		return false;
	}

#if WITH_EDITOR
	// Prevent texture compression and resizing
	Texture->Filter = TF_Nearest;
	Texture->MipGenSettings = TMGS_NoMipmaps;
	Texture->CompressionSettings = TC_VectorDisplacementmap; // No compression, preserves exact size
	Texture->SRGB = false;
	Texture->LODGroup = TEXTUREGROUP_World; // No size restrictions
	Texture->MaxTextureSize = 0; // No max size limit
	Texture->PowerOfTwoMode = ETexturePowerOfTwoSetting::None; // Allow non-power-of-2
	Texture->NeverStream = true; // Keep full resolution in memory
	
	UE_LOG(LogCharacterCreation, Warning, TEXT("Applied Paper2D texture settings to: %s"), *Texture->GetName());
	UE_LOG(LogCharacterCreation, Warning, TEXT("Texture size after settings: %dx%d"), Texture->GetSizeX(), Texture->GetSizeY());
	
	Texture->PostEditChange();
	Texture->MarkPackageDirty();
	
	return true;
#else
	UE_LOG(LogCharacterCreation, Error, TEXT("Texture setting modification is only available in editor builds"));
	return false;
#endif
}

TArray<UPaperSprite*> USpriteSheetProcessor::ExtractSprites(UTexture2D* Texture, const FSpriteSheetInfo& SpriteInfo)
{
	TArray<UPaperSprite*> ExtractedSprites;

	if (!Texture)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Texture is null"));
		return ExtractedSprites;
	}

#if WITH_EDITOR
	int32 TextureWidth = Texture->GetSizeX();
	int32 TextureHeight = Texture->GetSizeY();
	
	int32 SpriteWidth = TextureWidth / SpriteInfo.Columns;
	int32 SpriteHeight = TextureHeight / SpriteInfo.Rows;

	UE_LOG(LogCharacterCreation, Warning, TEXT("DEBUG: Actual texture dimensions: %dx%d"), TextureWidth, TextureHeight);
	UE_LOG(LogCharacterCreation, Warning, TEXT("DEBUG: Expected grid: %dx%d"), SpriteInfo.Columns, SpriteInfo.Rows);
	UE_LOG(LogCharacterCreation, Warning, TEXT("DEBUG: Calculated sprite size: %dx%d"), SpriteWidth, SpriteHeight);
	
	UE_LOG(LogCharacterCreation, Log, TEXT("Extracting sprites from %dx%d texture, sprite size: %dx%d"), 
		TextureWidth, TextureHeight, SpriteWidth, SpriteHeight);

	// Access the source texture data
	FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
	uint8* SourceData = static_cast<uint8*>(Mip.BulkData.Lock(LOCK_READ_ONLY));
	
	if (!SourceData)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Failed to lock texture data"));
		return ExtractedSprites;
	}

	// Determine pixel format size
	int32 BytesPerPixel = 4; // Assuming RGBA8
	EPixelFormat PixelFormat = Texture->GetPixelFormat();
	if (PixelFormat == PF_B8G8R8A8)
	{
		BytesPerPixel = 4;
	}
	else if (PixelFormat == PF_G8)
	{
		BytesPerPixel = 1;
	}
	else
	{
		UE_LOG(LogCharacterCreation, Warning, TEXT("Unsupported pixel format, assuming 4 bytes per pixel"));
	}

	for (int32 Row = 0; Row < SpriteInfo.Rows; Row++)
	{
		for (int32 Col = 0; Col < SpriteInfo.Columns; Col++)
		{
			FString SpriteName = FString::Printf(TEXT("%s_R%d_C%d"), *Texture->GetName(), Row, Col);
			FString PackagePath = FString::Printf(TEXT("/Game/Sprites/%s"), *SpriteName);
			
			// Create new texture for this sprite
			UTexture2D* SpriteTexture = CreateSpriteTexture(SourceData, TextureWidth, TextureHeight, 
				Col * SpriteWidth, Row * SpriteHeight, SpriteWidth, SpriteHeight, BytesPerPixel, SpriteName);
			
			if (!SpriteTexture)
			{
				UE_LOG(LogCharacterCreation, Error, TEXT("Failed to create sprite texture: %s"), *SpriteName);
				continue;
			}

			UPackage* SpritePackage = CreatePackage(*PackagePath);
			if (!SpritePackage)
			{
				UE_LOG(LogCharacterCreation, Error, TEXT("Failed to create sprite package: %s"), *PackagePath);
				continue;
			}

			UPaperSprite* NewSprite = NewObject<UPaperSprite>(SpritePackage, *SpriteName, RF_Public | RF_Standalone);
			if (!NewSprite)
			{
				UE_LOG(LogCharacterCreation, Error, TEXT("Failed to create sprite: %s"), *SpriteName);
				continue;
			}

			// Use reflection to access protected properties
			FProperty* SourceTextureProperty = UPaperSprite::StaticClass()->FindPropertyByName(TEXT("SourceTexture"));
			FProperty* SourceUVProperty = UPaperSprite::StaticClass()->FindPropertyByName(TEXT("SourceUV"));
			FProperty* SourceDimensionProperty = UPaperSprite::StaticClass()->FindPropertyByName(TEXT("SourceDimension"));
			
			if (SourceTextureProperty && SourceUVProperty && SourceDimensionProperty)
			{
				// Set source texture to the extracted sprite texture
				TSoftObjectPtr<UTexture2D>* SourceTexturePtr = SourceTextureProperty->ContainerPtrToValuePtr<TSoftObjectPtr<UTexture2D>>(NewSprite);
				if (SourceTexturePtr)
				{
					*SourceTexturePtr = SpriteTexture;
				}
				
				// Set source UV to origin since we have individual textures now
				FVector2D* SourceUVPtr = SourceUVProperty->ContainerPtrToValuePtr<FVector2D>(NewSprite);
				if (SourceUVPtr)
				{
					*SourceUVPtr = FVector2D::ZeroVector;
				}
				
				// Set source dimension to full texture size
				FVector2D* SourceDimensionPtr = SourceDimensionProperty->ContainerPtrToValuePtr<FVector2D>(NewSprite);
				if (SourceDimensionPtr)
				{
					*SourceDimensionPtr = FVector2D(SpriteWidth, SpriteHeight);
				}
				
				// Set pivot to center
				NewSprite->SetPivotMode(ESpritePivotMode::Center_Center, FVector2D::ZeroVector);
				
				// Rebuild the sprite data
				NewSprite->RebuildData();
			}
			else
			{
				UE_LOG(LogCharacterCreation, Error, TEXT("Failed to find sprite properties via reflection"));
			}
			
			NewSprite->PostEditChange();
			FAssetRegistryModule::AssetCreated(NewSprite);
			SpritePackage->MarkPackageDirty();
			
			// Save the sprite package to disk
			FSavePackageArgs SaveArgs;
			SaveArgs.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
			SaveArgs.Error = GError;
			SaveArgs.SaveFlags = SAVE_NoError;
			bool bSaved = UPackage::SavePackage(SpritePackage, NewSprite, 
				*FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension()), 
				SaveArgs);
			
			if (bSaved)
			{
				UE_LOG(LogCharacterCreation, Log, TEXT("✓ Saved sprite package to disk: %s"), *PackagePath);
			}
			else
			{
				UE_LOG(LogCharacterCreation, Warning, TEXT("✗ Failed to save sprite package: %s"), *PackagePath);
			}
			
			ExtractedSprites.Add(NewSprite);
			
			UE_LOG(LogCharacterCreation, Log, TEXT("Created sprite: %s at (%d, %d) size (%dx%d)"), 
				*SpriteName, Col * SpriteWidth, Row * SpriteHeight, SpriteWidth, SpriteHeight);
		}
	}

	// Unlock the source texture data
	Mip.BulkData.Unlock();

	UE_LOG(LogCharacterCreation, Log, TEXT("Extracted %d sprites total"), ExtractedSprites.Num());
#endif

	return ExtractedSprites;
}

TArray<UPaperFlipbook*> USpriteSheetProcessor::CreateAnimations(const TArray<UPaperSprite*>& Sprites, const FSpriteSheetInfo& SpriteInfo)
{
	TArray<UPaperFlipbook*> CreatedAnimations;

	if (Sprites.Num() != SpriteInfo.Rows * SpriteInfo.Columns)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Sprite count mismatch. Expected: %d, Got: %d"), 
			SpriteInfo.Rows * SpriteInfo.Columns, Sprites.Num());
		return CreatedAnimations;
	}

#if WITH_EDITOR
	for (int32 Row = 0; Row < SpriteInfo.Rows; Row++)
	{
		EAnimationType AnimType = static_cast<EAnimationType>(Row);
		FString AnimationName = GetAnimationName(AnimType);
		FString PackagePath = FString::Printf(TEXT("/Game/Animations/%s"), *AnimationName);
		
		UPackage* FlipbookPackage = CreatePackage(*PackagePath);
		if (!FlipbookPackage)
		{
			UE_LOG(LogCharacterCreation, Error, TEXT("Failed to create flipbook package: %s"), *PackagePath);
			continue;
		}

		UPaperFlipbook* NewFlipbook = NewObject<UPaperFlipbook>(FlipbookPackage, *AnimationName, RF_Public | RF_Standalone);
		if (!NewFlipbook)
		{
			UE_LOG(LogCharacterCreation, Error, TEXT("Failed to create flipbook: %s"), *AnimationName);
			continue;
		}

		for (int32 Col = 0; Col < SpriteInfo.Columns; Col++)
		{
			int32 SpriteIndex = Row * SpriteInfo.Columns + Col;
			if (SpriteIndex < Sprites.Num() && Sprites[SpriteIndex])
			{
				FPaperFlipbookKeyFrame KeyFrame;
				KeyFrame.Sprite = Sprites[SpriteIndex];
				KeyFrame.FrameRun = 1;
				
#if WITH_EDITOR
				// Use reflection to access protected KeyFrames array
				FProperty* KeyFramesProperty = UPaperFlipbook::StaticClass()->FindPropertyByName(TEXT("KeyFrames"));
				if (KeyFramesProperty)
				{
					TArray<FPaperFlipbookKeyFrame>* KeyFramesPtr = KeyFramesProperty->ContainerPtrToValuePtr<TArray<FPaperFlipbookKeyFrame>>(NewFlipbook);
					if (KeyFramesPtr)
					{
						KeyFramesPtr->Add(KeyFrame);
					}
				}
#endif
			}
		}

#if WITH_EDITOR
		// Use reflection to access protected FramesPerSecond property
		FProperty* FPSProperty = UPaperFlipbook::StaticClass()->FindPropertyByName(TEXT("FramesPerSecond"));
		if (FPSProperty)
		{
			float* FPSPtr = FPSProperty->ContainerPtrToValuePtr<float>(NewFlipbook);
			if (FPSPtr)
			{
				*FPSPtr = 12.0f;
			}
		}
		NewFlipbook->PostEditChange();
		FAssetRegistryModule::AssetCreated(NewFlipbook);
		FlipbookPackage->MarkPackageDirty();
		
		// Force save the package to disk
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
		SaveArgs.Error = GError;
		SaveArgs.SaveFlags = SAVE_NoError;
		UPackage::SavePackage(FlipbookPackage, NewFlipbook, 
			*FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension()), 
			SaveArgs);
		
		UE_LOG(LogCharacterCreation, Warning, TEXT("✓ Saved flipbook package to disk: %s"), *PackagePath);
#endif
		
		CreatedAnimations.Add(NewFlipbook);
		
		UE_LOG(LogCharacterCreation, Log, TEXT("Created animation: %s with %d frames"), 
*AnimationName, NewFlipbook->GetNumKeyFrames());
	}

	UE_LOG(LogCharacterCreation, Log, TEXT("Created %d animations total"), CreatedAnimations.Num());
#endif

	return CreatedAnimations;
}

FString USpriteSheetProcessor::GetAnimationName(EAnimationType AnimType) const
{
	switch (AnimType)
	{
		case EAnimationType::Idle:
			return TEXT("Idle");
		case EAnimationType::Move:
			return TEXT("Move");
		case EAnimationType::AttackSideways:
			return TEXT("AttackSideways");
		case EAnimationType::AttackSideways2:
			return TEXT("AttackSideways2");
		case EAnimationType::AttackDownwards:
			return TEXT("AttackDownwards");
		case EAnimationType::AttackDownwards2:
			return TEXT("AttackDownwards2");
		case EAnimationType::AttackUpwards:
			return TEXT("AttackUpwards");
		case EAnimationType::AttackUpwards2:
			return TEXT("AttackUpwards2");
		default:
			return TEXT("Unknown");
	}
}

UTexture2D* USpriteSheetProcessor::CreateSpriteTexture(uint8* SourceData, int32 SourceWidth, int32 SourceHeight, 
	int32 StartX, int32 StartY, int32 SpriteWidth, int32 SpriteHeight, int32 BytesPerPixel, const FString& SpriteName)
{
#if WITH_EDITOR
	FString PackageName = FString::Printf(TEXT("/Game/Textures/%s"), *SpriteName);
	FString AssetName = SpriteName;
	
	UPackage* Package = CreatePackage(*PackageName);
	if (!Package)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Failed to create texture package: %s"), *PackageName);
		return nullptr;
	}

	UTexture2D* NewTexture = NewObject<UTexture2D>(Package, *AssetName, RF_Public | RF_Standalone);
	if (!NewTexture)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Failed to create texture object: %s"), *AssetName);
		return nullptr;
	}

	// Initialize texture properties
	NewTexture->SetPlatformData(new FTexturePlatformData());
	NewTexture->GetPlatformData()->SizeX = SpriteWidth;
	NewTexture->GetPlatformData()->SizeY = SpriteHeight;
	NewTexture->GetPlatformData()->PixelFormat = PF_B8G8R8A8;

	// Create mip map
	FTexture2DMipMap* Mip = new FTexture2DMipMap();
	NewTexture->GetPlatformData()->Mips.Add(Mip);
	Mip->SizeX = SpriteWidth;
	Mip->SizeY = SpriteHeight;
	
	// Calculate size and allocate destination data
	int32 DataSize = SpriteWidth * SpriteHeight * BytesPerPixel;
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* DestData = static_cast<uint8*>(Mip->BulkData.Realloc(DataSize));

	// Copy pixel data from source texture region to destination
	for (int32 Y = 0; Y < SpriteHeight; Y++)
	{
		for (int32 X = 0; X < SpriteWidth; X++)
		{
			int32 SourceIndex = ((StartY + Y) * SourceWidth + (StartX + X)) * BytesPerPixel;
			int32 DestIndex = (Y * SpriteWidth + X) * BytesPerPixel;
			
			// Copy all channels (RGBA)
			for (int32 Channel = 0; Channel < BytesPerPixel; Channel++)
			{
				DestData[DestIndex + Channel] = SourceData[SourceIndex + Channel];
			}
		}
	}

	Mip->BulkData.Unlock();

	// Apply Paper2D texture settings
	NewTexture->Filter = TF_Nearest;
	NewTexture->MipGenSettings = TMGS_NoMipmaps;
	NewTexture->CompressionSettings = TC_EditorIcon;
	NewTexture->SRGB = false;
	NewTexture->LODGroup = TEXTUREGROUP_Pixels2D;
	NewTexture->MaxTextureSize = 0;
	NewTexture->PowerOfTwoMode = ETexturePowerOfTwoSetting::None;
	NewTexture->NeverStream = true;

	// Update and register the texture
	NewTexture->Source.Init(SpriteWidth, SpriteHeight, 1, 1, TSF_BGRA8, DestData);
	NewTexture->UpdateResource();
	NewTexture->PostEditChange();
	
	FAssetRegistryModule::AssetCreated(NewTexture);
	Package->MarkPackageDirty();

	// Save the texture package to disk
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
	SaveArgs.Error = GError;
	SaveArgs.SaveFlags = SAVE_NoError;
	bool bSaved = UPackage::SavePackage(Package, NewTexture, 
		*FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension()), 
		SaveArgs);
	
	if (bSaved)
	{
		UE_LOG(LogCharacterCreation, Log, TEXT("✓ Saved texture package to disk: %s"), *PackageName);
	}
	else
	{
		UE_LOG(LogCharacterCreation, Warning, TEXT("✗ Failed to save texture package: %s"), *PackageName);
	}

	UE_LOG(LogCharacterCreation, Log, TEXT("Created sprite texture: %s (%dx%d)"), *AssetName, SpriteWidth, SpriteHeight);
	return NewTexture;
#else
	UE_LOG(LogCharacterCreation, Error, TEXT("Texture creation is only available in editor builds"));
	return nullptr;
#endif
}

UInputAction* USpriteSheetProcessor::CreateInputAction(const FString& ActionName, const FString& PackagePath)
{
#if WITH_EDITOR
	UPackage* Package = CreatePackage(*PackagePath);
	if (!Package)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Failed to create input action package: %s"), *PackagePath);
		return nullptr;
	}

	UInputAction* NewInputAction = NewObject<UInputAction>(Package, *ActionName, RF_Public | RF_Standalone);
	if (!NewInputAction)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Failed to create input action: %s"), *ActionName);
		return nullptr;
	}

	// Set appropriate value type based on action name
	if (ActionName.Contains(TEXT("Move")))
	{
		NewInputAction->ValueType = EInputActionValueType::Axis2D;
	}
	else if (ActionName.Contains(TEXT("Attack")))
	{
		NewInputAction->ValueType = EInputActionValueType::Boolean;
	}
	else
	{
		NewInputAction->ValueType = EInputActionValueType::Boolean;
	}

	// Save the input action
	FAssetRegistryModule::AssetCreated(NewInputAction);
	Package->MarkPackageDirty();

	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
	SaveArgs.Error = GError;
	SaveArgs.SaveFlags = SAVE_NoError;
	bool bSaved = UPackage::SavePackage(Package, NewInputAction, 
		*FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension()), 
		SaveArgs);
	
	if (bSaved)
	{
		UE_LOG(LogCharacterCreation, Log, TEXT("✓ Saved input action to disk: %s"), *PackagePath);
	}
	else
	{
		UE_LOG(LogCharacterCreation, Warning, TEXT("✗ Failed to save input action: %s"), *PackagePath);
	}

	UE_LOG(LogCharacterCreation, Log, TEXT("Created input action: %s"), *ActionName);
	return NewInputAction;
#else
	UE_LOG(LogCharacterCreation, Error, TEXT("Input action creation is only available in editor builds"));
	return nullptr;
#endif
}

UInputMappingContext* USpriteSheetProcessor::CreateInputMappingContext(const FString& ContextName, const FString& PackagePath, UInputAction* MoveAction, UInputAction* AttackAction)
{
#if WITH_EDITOR
	if (!MoveAction || !AttackAction)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Cannot create input mapping context without valid input actions"));
		return nullptr;
	}

	UPackage* Package = CreatePackage(*PackagePath);
	if (!Package)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Failed to create input mapping context package: %s"), *PackagePath);
		return nullptr;
	}

	UInputMappingContext* NewMappingContext = NewObject<UInputMappingContext>(Package, *ContextName, RF_Public | RF_Standalone);
	if (!NewMappingContext)
	{
		UE_LOG(LogCharacterCreation, Error, TEXT("Failed to create input mapping context: %s"), *ContextName);
		return nullptr;
	}

	// Create mappings for WASD movement using the correct API
	const TArray<FEnhancedActionKeyMapping>& ConstMappings = NewMappingContext->GetMappings();
	TArray<FEnhancedActionKeyMapping>& Mappings = const_cast<TArray<FEnhancedActionKeyMapping>&>(ConstMappings);

	// WASD Forward (W key)
	FEnhancedActionKeyMapping& WASDForward = Mappings.AddDefaulted_GetRef();
	WASDForward.Action = MoveAction;
	WASDForward.Key = EKeys::W;
	UInputModifierSwizzleAxis* WSwizzle = NewObject<UInputModifierSwizzleAxis>(NewMappingContext);
	WSwizzle->Order = EInputAxisSwizzle::YXZ;
	WASDForward.Modifiers.Add(WSwizzle);

	// WASD Backward (S key)  
	FEnhancedActionKeyMapping& WASDBackward = Mappings.AddDefaulted_GetRef();
	WASDBackward.Action = MoveAction;
	WASDBackward.Key = EKeys::S;
	UInputModifierSwizzleAxis* SSwizzle = NewObject<UInputModifierSwizzleAxis>(NewMappingContext);
	SSwizzle->Order = EInputAxisSwizzle::YXZ;
	UInputModifierNegate* SNegate = NewObject<UInputModifierNegate>(NewMappingContext);
	WASDBackward.Modifiers.Add(SSwizzle);
	WASDBackward.Modifiers.Add(SNegate);

	// WASD Left (A key)
	FEnhancedActionKeyMapping& WASDLeft = Mappings.AddDefaulted_GetRef();
	WASDLeft.Action = MoveAction;
	WASDLeft.Key = EKeys::A;
	UInputModifierNegate* ANegate = NewObject<UInputModifierNegate>(NewMappingContext);
	WASDLeft.Modifiers.Add(ANegate);

	// WASD Right (D key)
	FEnhancedActionKeyMapping& WASDRight = Mappings.AddDefaulted_GetRef();
	WASDRight.Action = MoveAction;
	WASDRight.Key = EKeys::D;

	// Left Mouse Attack
	FEnhancedActionKeyMapping& LeftMouseAttack = Mappings.AddDefaulted_GetRef();
	LeftMouseAttack.Action = AttackAction;
	LeftMouseAttack.Key = EKeys::LeftMouseButton;

	// Save the input mapping context
	FAssetRegistryModule::AssetCreated(NewMappingContext);
	Package->MarkPackageDirty();

	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
	SaveArgs.Error = GError;
	SaveArgs.SaveFlags = SAVE_NoError;
	bool bSaved = UPackage::SavePackage(Package, NewMappingContext, 
		*FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension()), 
		SaveArgs);
	
	if (bSaved)
	{
		UE_LOG(LogCharacterCreation, Log, TEXT("✓ Saved input mapping context to disk: %s"), *PackagePath);
	}
	else
	{
		UE_LOG(LogCharacterCreation, Warning, TEXT("✗ Failed to save input mapping context: %s"), *PackagePath);
	}

	UE_LOG(LogCharacterCreation, Log, TEXT("Created input mapping context: %s with %d mappings"), *ContextName, NewMappingContext->GetMappings().Num());
	return NewMappingContext;
#else
	UE_LOG(LogCharacterCreation, Error, TEXT("Input mapping context creation is only available in editor builds"));
	return nullptr;
#endif
}