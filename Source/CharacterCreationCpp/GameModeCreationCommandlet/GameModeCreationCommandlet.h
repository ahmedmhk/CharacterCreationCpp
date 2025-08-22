#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "GameModeCreationCommandlet.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGameModeCreation, Log, All);

UCLASS()
class CHARACTERCREATIONCPP_API UGameModeCreationCommandlet : public UCommandlet
{
    GENERATED_BODY()

public:
    UGameModeCreationCommandlet();

    virtual int32 Main(const FString& Params) override;

private:
    bool CreateGameModeClass();
    bool SetDefaultGameMode();
    bool UpdateDefaultEngineIni();
    
    FString GenerateGameModeHeader();
    FString GenerateGameModeCpp();
    
    bool WriteSourceFile(const FString& FilePath, const FString& Content);
};