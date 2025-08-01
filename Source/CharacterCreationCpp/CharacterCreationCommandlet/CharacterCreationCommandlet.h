#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "SpriteSheetProcessor.h"
#include "CharacterCreationCommandlet.generated.h"

UCLASS()
class CHARACTERCREATIONCPP_API UCharacterCreationCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UCharacterCreationCommandlet();

	virtual int32 Main(const FString& Params) override;

	// Default sprite sheet grid dimensions
	static constexpr int32 DefaultColumns = 6;
	static constexpr int32 DefaultRows = 8;
	static constexpr int32 MaxGridDimension = 100;

private:
	// Single sprite processing
	bool ProcessSpriteSheetFromCommandline(const FString& TextureName, const FSpriteSheetInfo& SpriteInfo);
	
	// Batch processing
	bool BatchProcessSpriteSheets(const FSpriteSheetInfo& SpriteInfo, bool bCreateCharacters, bool bDryRun = false);
	
	// Character generation
	bool GenerateCharacterClass(const FString& CharacterName, const FString& TextureName);
	bool WriteCharacterHeaderFile(const FString& CharacterName, const FString& TextureName);
	bool WriteCharacterSourceFile(const FString& CharacterName, const FString& TextureName);
	
	// Utility
	void PrintUsage() const;
	void PrintSuccess(const FString& TextureName) const;
	void PrintFailure(const FString& TextureName) const;
	void PrintBatchSummary(const TArray<FString>& ProcessedTextures, const TArray<FString>& GeneratedCharacters) const;
	bool ValidateAndSanitizePath(FString& Path) const;
	bool ValidateAssetReferences() const;
	void PrintDryRunSummary(const TArray<FString>& FilesToProcess) const;
};