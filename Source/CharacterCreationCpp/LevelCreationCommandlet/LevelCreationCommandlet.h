#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "LevelCreationCommandlet.generated.h"

UCLASS()
class CHARACTERCREATIONCPP_API ULevelCreationCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	ULevelCreationCommandlet();

	virtual int32 Main(const FString& Params) override;

private:
	// Level creation
	bool CreateLevel(const FString& MapName, const FString& OutputPath, bool bDryRun);
	bool SaveLevel(class UWorld* World, const FString& PackageName);
	
	// Actor spawning
	bool SpawnEnvironmentalActors(class UWorld* World);
	class ADirectionalLight* SpawnDirectionalLight(class UWorld* World);
	class ASkyLight* SpawnSkyLight(class UWorld* World);
	class ASkyAtmosphere* SpawnSkyAtmosphere(class UWorld* World);
	class AVolumetricCloud* SpawnVolumetricCloud(class UWorld* World);
	class AExponentialHeightFog* SpawnExponentialHeightFog(class UWorld* World);
	
	// Utility
	void PrintUsage() const;
	void PrintSuccess(const FString& MapName, const FString& PackagePath) const;
	void PrintFailure(const FString& MapName) const;
	void PrintDryRunSummary(const FString& MapName, const FString& OutputPath) const;
	bool ValidateAndSanitizePath(FString& Path) const;
};