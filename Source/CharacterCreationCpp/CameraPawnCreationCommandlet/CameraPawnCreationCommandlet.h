#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "CameraPawnCreationCommandlet.generated.h"

UCLASS()
class CHARACTERCREATIONCPP_API UCameraPawnCreationCommandlet : public UCommandlet
{
    GENERATED_BODY()

public:
    UCameraPawnCreationCommandlet();

    virtual int32 Main(const FString& Params) override;

private:
    bool CreateCameraPawnClass();
    bool CreateInputActions();
    bool CreateInputMappingContext();
    
    FString GenerateCameraPawnHeader();
    FString GenerateCameraPawnCpp();
    
    bool WriteSourceFile(const FString& FilePath, const FString& Content);
    bool CreateInputAction(const FString& ActionName, const FString& PackagePath, uint8 ValueType);
    
    class UInputAction* ZoomAction;
    class UInputAction* RotateAction;
    
    void PrintUsage() const;
    void PrintSuccess() const;
};