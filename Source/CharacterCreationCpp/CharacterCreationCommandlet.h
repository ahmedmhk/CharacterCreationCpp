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

private:
	bool ProcessSpriteSheetFromCommandline(const FString& TextureName, const FSpriteSheetInfo& SpriteInfo);
	void PrintUsage() const;
	void PrintSuccess(const FString& TextureName) const;
	void PrintFailure(const FString& TextureName) const;
};