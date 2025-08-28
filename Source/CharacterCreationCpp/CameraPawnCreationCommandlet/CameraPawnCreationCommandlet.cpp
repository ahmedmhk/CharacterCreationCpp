#include "CameraPawnCreationCommandlet.h"
#include "CameraPawnCreationLog.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/Paths.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "EnhancedInput/Public/InputModifiers.h"
#include "Engine/Engine.h"
#include "Modules/ModuleManager.h"
#include "Misc/PackageName.h"

UCameraPawnCreationCommandlet::UCameraPawnCreationCommandlet()
{
    IsClient = false;
    IsEditor = true;
    IsServer = false;
    LogToConsole = true;
    ShowErrorCount = true;
    HelpDescription = TEXT("Creates a camera pawn with spring arm and input actions");
    HelpUsage = TEXT("CameraPawnCreationCommandlet");
}

int32 UCameraPawnCreationCommandlet::Main(const FString& Params)
{
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("=== Camera Pawn Creation Commandlet Started ==="));
    
    bool bSuccess = true;
    
    // Create the CameraPawn C++ class
    if (!CreateCameraPawnClass())
    {
        UE_LOG(LogCameraPawnCreation, Error, TEXT("Failed to create CameraPawn class"));
        bSuccess = false;
    }
    
    // Create input actions
    if (!CreateInputActions())
    {
        UE_LOG(LogCameraPawnCreation, Error, TEXT("Failed to create input actions"));
        bSuccess = false;
    }
    
    // Create input mapping context
    if (!CreateInputMappingContext())
    {
        UE_LOG(LogCameraPawnCreation, Error, TEXT("Failed to create input mapping context"));
        bSuccess = false;
    }
    
    if (bSuccess)
    {
        PrintSuccess();
        UE_LOG(LogCameraPawnCreation, Warning, TEXT("=== Camera Pawn Creation Commandlet Completed Successfully ==="));
        return 0;
    }
    else
    {
        UE_LOG(LogCameraPawnCreation, Error, TEXT("=== Camera Pawn Creation Commandlet Failed ==="));
        return 1;
    }
}

bool UCameraPawnCreationCommandlet::CreateCameraPawnClass()
{
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("Creating CameraPawn class..."));
    
    FString ProjectDir = FPaths::ProjectDir();
    FString SourceDir = ProjectDir / TEXT("Source") / TEXT("CharacterCreationCpp") / TEXT("CameraPawnCreationCommandlet");
    
    // Write header file
    FString HeaderPath = SourceDir / TEXT("CameraPawn.h");
    FString HeaderContent = GenerateCameraPawnHeader();
    if (!WriteSourceFile(HeaderPath, HeaderContent))
    {
        UE_LOG(LogCameraPawnCreation, Error, TEXT("Failed to write CameraPawn.h"));
        return false;
    }
    
    // Write cpp file
    FString CppPath = SourceDir / TEXT("CameraPawn.cpp");
    FString CppContent = GenerateCameraPawnCpp();
    if (!WriteSourceFile(CppPath, CppContent))
    {
        UE_LOG(LogCameraPawnCreation, Error, TEXT("Failed to write CameraPawn.cpp"));
        return false;
    }
    
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("✓ CameraPawn class created successfully"));
    return true;
}

FString UCameraPawnCreationCommandlet::GenerateCameraPawnHeader()
{
    return TEXT("#pragma once\n")
           TEXT("\n")
           TEXT("#include \"CoreMinimal.h\"\n")
           TEXT("#include \"GameFramework/Pawn.h\"\n")
           TEXT("#include \"InputActionValue.h\"\n")
           TEXT("#include \"CameraPawn.generated.h\"\n")
           TEXT("\n")
           TEXT("class USpringArmComponent;\n")
           TEXT("class UCameraComponent;\n")
           TEXT("class UFloatingPawnMovement;\n")
           TEXT("class UInputMappingContext;\n")
           TEXT("class UInputAction;\n")
           TEXT("\n")
           TEXT("UCLASS()\n")
           TEXT("class CHARACTERCREATIONCPP_API ACameraPawn : public APawn\n")
           TEXT("{\n")
           TEXT("    GENERATED_BODY()\n")
           TEXT("\n")
           TEXT("public:\n")
           TEXT("    ACameraPawn();\n")
           TEXT("    virtual void Tick(float DeltaTime) override;\n")
           TEXT("\n")
           TEXT("protected:\n")
           TEXT("    virtual void BeginPlay() override;\n")
           TEXT("    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;\n")
           TEXT("\n")
           TEXT("    // Components\n")
           TEXT("    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = \"Camera\")\n")
           TEXT("    USpringArmComponent* SpringArmComponent;\n")
           TEXT("\n")
           TEXT("    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = \"Camera\")\n")
           TEXT("    UCameraComponent* CameraComponent;\n")
           TEXT("\n")
           TEXT("    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = \"Movement\")\n")
           TEXT("    UFloatingPawnMovement* FloatingPawnMovement;\n")
           TEXT("\n")
           TEXT("    // Input - Using direct references for reliability\n")
           TEXT("    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = \"Input\")\n")
           TEXT("    UInputMappingContext* CameraMappingContext;\n")
           TEXT("\n")
           TEXT("    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = \"Input\")\n")
           TEXT("    UInputAction* ZoomAction;\n")
           TEXT("\n")
           TEXT("    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = \"Input\")\n")
           TEXT("    UInputAction* RotateAction;\n")
           TEXT("\n")
           TEXT("    // Input callbacks\n")
           TEXT("    void Zoom(const FInputActionValue& Value);\n")
           TEXT("    void Rotate(const FInputActionValue& Value);\n")
           TEXT("\n")
           TEXT("    // Camera settings\n")
           TEXT("    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = \"Camera\")\n")
           TEXT("    float ZoomSpeed;\n")
           TEXT("\n")
           TEXT("    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = \"Camera\")\n")
           TEXT("    float MinZoomDistance;\n")
           TEXT("\n")
           TEXT("    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = \"Camera\")\n")
           TEXT("    float MaxZoomDistance;\n")
           TEXT("\n")
           TEXT("    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = \"Camera\")\n")
           TEXT("    float RotationSpeed;\n")
           TEXT("\n")
           TEXT("    // Smooth interpolation\n")
           TEXT("    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = \"Camera\", meta = (ClampMin = \"0.1\", ClampMax = \"20.0\"))\n")
           TEXT("    float ZoomSmoothSpeed = 8.0f;\n")
           TEXT("\n")
           TEXT("    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = \"Camera\", meta = (ClampMin = \"0.1\", ClampMax = \"20.0\"))\n")
           TEXT("    float RotationSmoothSpeed = 10.0f;\n")
           TEXT("\n")
           TEXT("private:\n")
           TEXT("    // Target values for smooth interpolation\n")
           TEXT("    float TargetArmLength;\n")
           TEXT("    FRotator TargetRotation;\n")
           TEXT("    float ZoomInputValue;\n")
           TEXT("    FVector2D RotationInputValue;\n")
           TEXT("};\n");
}

FString UCameraPawnCreationCommandlet::GenerateCameraPawnCpp()
{
    return TEXT("#include \"CameraPawn.h\"\n")
           TEXT("#include \"GameFramework/SpringArmComponent.h\"\n")
           TEXT("#include \"Camera/CameraComponent.h\"\n")
           TEXT("#include \"GameFramework/FloatingPawnMovement.h\"\n")
           TEXT("#include \"Components/InputComponent.h\"\n")
           TEXT("#include \"EnhancedInputComponent.h\"\n")
           TEXT("#include \"EnhancedInputSubsystems.h\"\n")
           TEXT("#include \"InputMappingContext.h\"\n")
           TEXT("#include \"InputAction.h\"\n")
           TEXT("#include \"Engine/LocalPlayer.h\"\n")
           TEXT("#include \"GameFramework/PlayerController.h\"\n")
           TEXT("\n")
           TEXT("ACameraPawn::ACameraPawn()\n")
           TEXT("{\n")
           TEXT("    PrimaryActorTick.bCanEverTick = true;\n")
           TEXT("\n")
           TEXT("    // Create root component\n")
           TEXT("    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT(\"RootComponent\"));\n")
           TEXT("\n")
           TEXT("    // Create spring arm component\n")
           TEXT("    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT(\"SpringArm\"));\n")
           TEXT("    SpringArmComponent->SetupAttachment(RootComponent);\n")
           TEXT("    SpringArmComponent->TargetArmLength = 700.0f;\n")
           TEXT("    SpringArmComponent->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));\n")
           TEXT("    SpringArmComponent->bDoCollisionTest = false;\n")
           TEXT("    SpringArmComponent->bUsePawnControlRotation = false;\n")
           TEXT("\n")
           TEXT("    // Create camera component\n")
           TEXT("    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT(\"Camera\"));\n")
           TEXT("    CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);\n")
           TEXT("    CameraComponent->SetProjectionMode(ECameraProjectionMode::Perspective);\n")
           TEXT("    CameraComponent->SetFieldOfView(90.0f);\n")
           TEXT("\n")
           TEXT("    // Create floating pawn movement for basic movement\n")
           TEXT("    FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT(\"FloatingPawnMovement\"));\n")
           TEXT("    FloatingPawnMovement->MaxSpeed = 600.0f;\n")
           TEXT("    FloatingPawnMovement->Acceleration = 4000.0f;\n")
           TEXT("    FloatingPawnMovement->Deceleration = 8000.0f;\n")
           TEXT("\n")
           TEXT("    // Set default camera settings\n")
           TEXT("    ZoomSpeed = 100.0f;  // Increased for more responsive mouse wheel zooming\n")
           TEXT("    MinZoomDistance = 200.0f;\n")
           TEXT("    MaxZoomDistance = 2000.0f;\n")
           TEXT("    RotationSpeed = 100.0f;\n")
           TEXT("    \n")
           TEXT("    // Initialize target values\n")
           TEXT("    TargetArmLength = 700.0f;\n")
           TEXT("    TargetRotation = FRotator(-60.0f, 0.0f, 0.0f);\n")
           TEXT("    ZoomInputValue = 0.0f;\n")
           TEXT("    RotationInputValue = FVector2D::ZeroVector;\n")
           TEXT("\n")
           TEXT("    // Load input assets directly in constructor\n")
           TEXT("    static ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContextFinder(TEXT(\"/Game/Input/IMC_CameraControl\"));\n")
           TEXT("    if (MappingContextFinder.Succeeded())\n")
           TEXT("    {\n")
           TEXT("        CameraMappingContext = MappingContextFinder.Object;\n")
           TEXT("    }\n")
           TEXT("    \n")
           TEXT("    static ConstructorHelpers::FObjectFinder<UInputAction> ZoomActionFinder(TEXT(\"/Game/Input/IA_Zoom\"));\n")
           TEXT("    if (ZoomActionFinder.Succeeded())\n")
           TEXT("    {\n")
           TEXT("        ZoomAction = ZoomActionFinder.Object;\n")
           TEXT("    }\n")
           TEXT("    \n")
           TEXT("    static ConstructorHelpers::FObjectFinder<UInputAction> RotateActionFinder(TEXT(\"/Game/Input/IA_Rotate\"));\n")
           TEXT("    if (RotateActionFinder.Succeeded())\n")
           TEXT("    {\n")
           TEXT("        RotateAction = RotateActionFinder.Object;\n")
           TEXT("    }\n")
           TEXT("}\n")
           TEXT("\n")
           TEXT("void ACameraPawn::Tick(float DeltaTime)\n")
           TEXT("{\n")
           TEXT("    Super::Tick(DeltaTime);\n")
           TEXT("    \n")
           TEXT("    if (SpringArmComponent)\n")
           TEXT("    {\n")
           TEXT("        // Smoothly interpolate zoom\n")
           TEXT("        float CurrentArmLength = SpringArmComponent->TargetArmLength;\n")
           TEXT("        float NewArmLength = FMath::FInterpTo(CurrentArmLength, TargetArmLength, DeltaTime, ZoomSmoothSpeed);\n")
           TEXT("        SpringArmComponent->TargetArmLength = NewArmLength;\n")
           TEXT("        \n")
           TEXT("        // Smoothly interpolate rotation\n")
           TEXT("        FRotator CurrentRotation = SpringArmComponent->GetRelativeRotation();\n")
           TEXT("        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSmoothSpeed);\n")
           TEXT("        SpringArmComponent->SetRelativeRotation(NewRotation);\n")
           TEXT("    }\n")
           TEXT("}\n")
           TEXT("\n")
           TEXT("void ACameraPawn::BeginPlay()\n")
           TEXT("{\n")
           TEXT("    Super::BeginPlay();\n")
           TEXT("    \n")
           TEXT("    // Initialize target values from current spring arm settings\n")
           TEXT("    if (SpringArmComponent)\n")
           TEXT("    {\n")
           TEXT("        TargetArmLength = SpringArmComponent->TargetArmLength;\n")
           TEXT("        TargetRotation = SpringArmComponent->GetRelativeRotation();\n")
           TEXT("    }\n")
           TEXT("\n")
           TEXT("    if (GEngine)\n")
           TEXT("    {\n")
           TEXT("        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT(\"CameraPawn::BeginPlay() - Starting input setup\"));\n")
           TEXT("    }\n")
           TEXT("\n")
           TEXT("    // Add input mapping context\n")
           TEXT("    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))\n")
           TEXT("    {\n")
           TEXT("        if (GEngine)\n")
           TEXT("        {\n")
           TEXT("            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT(\"CameraPawn - Found PlayerController\"));\n")
           TEXT("        }\n")
           TEXT("\n")
           TEXT("        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))\n")
           TEXT("        {\n")
           TEXT("            if (GEngine)\n")
           TEXT("            {\n")
           TEXT("                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT(\"CameraPawn - Found Enhanced Input Subsystem\"));\n")
           TEXT("            }\n")
           TEXT("\n")
           TEXT("            // Load and add mapping context if it exists\n")
           TEXT("            if (CameraMappingContext)\n")
           TEXT("            {\n")
           TEXT("                if (GEngine)\n")
           TEXT("                {\n")
           TEXT("                    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, \n")
           TEXT("                        TEXT(\"CameraPawn - InputMappingContext loaded successfully\"));\n")
           TEXT("                }\n")
           TEXT("\n")
           TEXT("                // Use the directly loaded context\n")
           TEXT("                if (CameraMappingContext)\n")
           TEXT("                {\n")
           TEXT("                    Subsystem->AddMappingContext(CameraMappingContext, 0);\n")
           TEXT("                    if (GEngine)\n")
           TEXT("                    {\n")
           TEXT("                        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, \n")
           TEXT("                            FString::Printf(TEXT(\"CameraPawn - Successfully added IMC: %s\"), *CameraMappingContext->GetName()));\n")
           TEXT("                    }\n")
           TEXT("                }\n")
           TEXT("                else\n")
           TEXT("                {\n")
           TEXT("                    if (GEngine)\n")
           TEXT("                    {\n")
           TEXT("                        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT(\"CameraPawn - FAILED to load InputMappingContext!\"));\n")
           TEXT("                    }\n")
           TEXT("                }\n")
           TEXT("            }\n")
           TEXT("            else\n")
           TEXT("            {\n")
           TEXT("                if (GEngine)\n")
           TEXT("                {\n")
           TEXT("                    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT(\"CameraPawn - CameraMappingContext is NULL!\"));\n")
           TEXT("                }\n")
           TEXT("            }\n")
           TEXT("        }\n")
           TEXT("        else\n")
           TEXT("        {\n")
           TEXT("            if (GEngine)\n")
           TEXT("            {\n")
           TEXT("                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT(\"CameraPawn - Failed to get Enhanced Input Subsystem!\"));\n")
           TEXT("            }\n")
           TEXT("        }\n")
           TEXT("    }\n")
           TEXT("    else\n")
           TEXT("    {\n")
           TEXT("        if (GEngine)\n")
           TEXT("        {\n")
           TEXT("            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT(\"CameraPawn - No PlayerController found!\"));\n")
           TEXT("        }\n")
           TEXT("    }\n")
           TEXT("}\n")
           TEXT("\n")
           TEXT("void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)\n")
           TEXT("{\n")
           TEXT("    Super::SetupPlayerInputComponent(PlayerInputComponent);\n")
           TEXT("\n")
           TEXT("    if (GEngine)\n")
           TEXT("    {\n")
           TEXT("        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT(\"CameraPawn::SetupPlayerInputComponent() - Setting up input bindings\"));\n")
           TEXT("    }\n")
           TEXT("\n")
           TEXT("    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))\n")
           TEXT("    {\n")
           TEXT("        if (GEngine)\n")
           TEXT("        {\n")
           TEXT("            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT(\"CameraPawn - Got EnhancedInputComponent\"));\n")
           TEXT("        }\n")
           TEXT("\n")
           TEXT("        // Bind zoom action if it exists\n")
           TEXT("        if (ZoomAction)\n")
           TEXT("        {\n")
           TEXT("            EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ACameraPawn::Zoom);\n")
           TEXT("            if (GEngine)\n")
           TEXT("            {\n")
           TEXT("                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, \n")
           TEXT("                    FString::Printf(TEXT(\"CameraPawn - Successfully bound Zoom Action: %s\"), *ZoomAction->GetName()));\n")
           TEXT("            }\n")
           TEXT("        }\n")
           TEXT("        else\n")
           TEXT("        {\n")
           TEXT("            if (GEngine)\n")
           TEXT("            {\n")
           TEXT("                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT(\"CameraPawn - ZoomAction is NULL!\"));\n")
           TEXT("            }\n")
           TEXT("        }\n")
           TEXT("\n")
           TEXT("        // Bind rotate action if it exists\n")
           TEXT("        if (RotateAction)\n")
           TEXT("        {\n")
           TEXT("            EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this, &ACameraPawn::Rotate);\n")
           TEXT("            if (GEngine)\n")
           TEXT("            {\n")
           TEXT("                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, \n")
           TEXT("                    FString::Printf(TEXT(\"CameraPawn - Successfully bound Rotate Action: %s\"), *RotateAction->GetName()));\n")
           TEXT("            }\n")
           TEXT("        }\n")
           TEXT("        else\n")
           TEXT("        {\n")
           TEXT("            if (GEngine)\n")
           TEXT("            {\n")
           TEXT("                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT(\"CameraPawn - RotateAction is NULL!\"));\n")
           TEXT("            }\n")
           TEXT("        }\n")
           TEXT("    }\n")
           TEXT("    else\n")
           TEXT("    {\n")
           TEXT("        if (GEngine)\n")
           TEXT("        {\n")
           TEXT("            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT(\"CameraPawn - Failed to get EnhancedInputComponent!\"));\n")
           TEXT("        }\n")
           TEXT("    }\n")
           TEXT("}\n")
           TEXT("\n")
           TEXT("void ACameraPawn::Zoom(const FInputActionValue& Value)\n")
           TEXT("{\n")
           TEXT("    const float ZoomValue = Value.Get<float>();\n")
           TEXT("    ZoomInputValue = ZoomValue;\n")
           TEXT("    \n")
           TEXT("    // Determine if input is from controller (continuous) or mouse wheel (discrete)\n")
           TEXT("    // Controller input is typically in range -1 to 1, mouse wheel is usually larger discrete values\n")
           TEXT("    float ZoomStep = 0.0f;\n")
           TEXT("    \n")
           TEXT("    if (FMath::Abs(ZoomValue) <= 1.0f)\n")
           TEXT("    {\n")
           TEXT("        // Controller input: scale to 2000 units per second at full deflection\n")
           TEXT("        ZoomStep = ZoomValue * 2000.0f * GetWorld()->GetDeltaSeconds();\n")
           TEXT("    }\n")
           TEXT("    else\n")
           TEXT("    {\n")
           TEXT("        // Mouse wheel input: use full zoom speed for discrete steps\n")
           TEXT("        ZoomStep = ZoomValue * ZoomSpeed;\n")
           TEXT("    }\n")
           TEXT("    \n")
           TEXT("    TargetArmLength = FMath::Clamp(TargetArmLength - ZoomStep, MinZoomDistance, MaxZoomDistance);\n")
           TEXT("    \n")
           TEXT("    // Debug message to check if zoom input is being received\n")
           TEXT("    if (GEngine && FMath::Abs(ZoomValue) > 0.01f)\n")
           TEXT("    {\n")
           TEXT("        GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, \n")
           TEXT("            FString::Printf(TEXT(\"Zoom Input: %f | Target: %.1f | Step: %.1f\"), ZoomValue, TargetArmLength, ZoomStep));\n")
           TEXT("    }\n")
           TEXT("}\n")
           TEXT("\n")
           TEXT("void ACameraPawn::Rotate(const FInputActionValue& Value)\n")
           TEXT("{\n")
           TEXT("    const FVector2D RotationValue = Value.Get<FVector2D>();\n")
           TEXT("    RotationInputValue = RotationValue;\n")
           TEXT("    \n")
           TEXT("    // Update target rotation based on input\n")
           TEXT("    float DeltaTime = GetWorld()->GetDeltaSeconds();\n")
           TEXT("    \n")
           TEXT("    // Apply yaw (horizontal) rotation - inverted for natural camera control\n")
           TEXT("    TargetRotation.Yaw += RotationValue.X * RotationSpeed * DeltaTime;\n")
           TEXT("    \n")
           TEXT("    // Apply pitch (vertical) rotation with clamping - inverted for natural camera control\n")
           TEXT("    float NewPitch = TargetRotation.Pitch + (RotationValue.Y * RotationSpeed * DeltaTime);\n")
           TEXT("    TargetRotation.Pitch = FMath::Clamp(NewPitch, -80.0f, -10.0f); // Limit vertical rotation\n")
           TEXT("    \n")
           TEXT("    // Debug message for rotation input\n")
           TEXT("    if (GEngine && RotationValue.Size() > 0.01f)\n")
           TEXT("    {\n")
           TEXT("        GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, \n")
           TEXT("            FString::Printf(TEXT(\"Rotation Input: X=%.2f Y=%.2f | Target: P=%.1f Y=%.1f\"), \n")
           TEXT("                RotationValue.X, RotationValue.Y, TargetRotation.Pitch, TargetRotation.Yaw));\n")
           TEXT("    }\n")
           TEXT("}\n");
}

bool UCameraPawnCreationCommandlet::CreateInputActions()
{
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("Creating input actions..."));
    
    // Create IA_Zoom
    if (!CreateInputAction(TEXT("IA_Zoom"), TEXT("/Game/Input/IA_Zoom"), 1)) // Axis1D
    {
        UE_LOG(LogCameraPawnCreation, Error, TEXT("Failed to create IA_Zoom"));
        return false;
    }
    
    // Create IA_Rotate
    if (!CreateInputAction(TEXT("IA_Rotate"), TEXT("/Game/Input/IA_Rotate"), 3)) // Axis2D
    {
        UE_LOG(LogCameraPawnCreation, Error, TEXT("Failed to create IA_Rotate"));
        return false;
    }
    
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("✓ Input actions created successfully"));
    return true;
}

bool UCameraPawnCreationCommandlet::CreateInputAction(const FString& ActionName, const FString& PackagePath, uint8 ValueType)
{
#if WITH_EDITOR
    UPackage* Package = CreatePackage(*PackagePath);
    if (!Package)
    {
        UE_LOG(LogCameraPawnCreation, Error, TEXT("Failed to create input action package: %s"), *PackagePath);
        return false;
    }

    UInputAction* NewInputAction = NewObject<UInputAction>(Package, *ActionName, RF_Public | RF_Standalone);
    if (!NewInputAction)
    {
        UE_LOG(LogCameraPawnCreation, Error, TEXT("Failed to create input action: %s"), *ActionName);
        return false;
    }

    // Set value type
    // 0 = Boolean, 1 = Axis1D, 2 = Axis2D, 3 = Axis3D
    NewInputAction->ValueType = (EInputActionValueType)ValueType;

    // Save to keep reference
    if (ActionName == TEXT("IA_Zoom"))
    {
        ZoomAction = NewInputAction;
    }
    else if (ActionName == TEXT("IA_Rotate"))
    {
        RotateAction = NewInputAction;
    }

    // Mark the package and register the asset
    FAssetRegistryModule::AssetCreated(NewInputAction);
    Package->MarkPackageDirty();
    Package->SetDirtyFlag(true);
    
    // Ensure the package is fully loaded
    Package->FullyLoad();

    // Configure save arguments
    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
    SaveArgs.Error = GError;
    SaveArgs.SaveFlags = SAVE_NoError;
    
    // Get the full file path
    FString PackageFilename = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());
    
    // Ensure directory exists
    FString Directory = FPaths::GetPath(PackageFilename);
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.DirectoryExists(*Directory))
    {
        PlatformFile.CreateDirectoryTree(*Directory);
    }
    
    // Save the package
    bool bSaved = UPackage::SavePackage(Package, NewInputAction, *PackageFilename, SaveArgs);
    
    if (bSaved)
    {
        UE_LOG(LogCameraPawnCreation, Warning, TEXT("✓ Successfully saved input action: %s to %s"), *ActionName, *PackageFilename);
        
        // Force asset registry to update
        FAssetRegistryModule& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
        AssetRegistry.Get().AssetCreated(NewInputAction);
        AssetRegistry.Get().SearchAllAssets(true);
    }
    else
    {
        UE_LOG(LogCameraPawnCreation, Error, TEXT("✗ Failed to save input action: %s to %s"), *ActionName, *PackageFilename);
        return false;
    }

    return true;
#else
    UE_LOG(LogCameraPawnCreation, Error, TEXT("Input action creation is only available in editor builds"));
    return false;
#endif
}

bool UCameraPawnCreationCommandlet::CreateInputMappingContext()
{
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("Creating input mapping context..."));
    
#if WITH_EDITOR
    // Load or use the created actions
    if (!ZoomAction)
    {
        ZoomAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/IA_Zoom"));
    }
    if (!RotateAction)
    {
        RotateAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/IA_Rotate"));
    }
    
    if (!ZoomAction || !RotateAction)
    {
        UE_LOG(LogCameraPawnCreation, Error, TEXT("Cannot create input mapping context without valid input actions"));
        return false;
    }

    FString PackagePath = TEXT("/Game/Input/IMC_CameraControl");
    UPackage* Package = CreatePackage(*PackagePath);
    if (!Package)
    {
        UE_LOG(LogCameraPawnCreation, Error, TEXT("Failed to create input mapping context package"));
        return false;
    }

    UInputMappingContext* NewMappingContext = NewObject<UInputMappingContext>(Package, TEXT("IMC_CameraControl"), RF_Public | RF_Standalone);
    if (!NewMappingContext)
    {
        UE_LOG(LogCameraPawnCreation, Error, TEXT("Failed to create input mapping context"));
        return false;
    }

    // Create mappings
    const TArray<FEnhancedActionKeyMapping>& ConstMappings = NewMappingContext->GetMappings();
    TArray<FEnhancedActionKeyMapping>& Mappings = const_cast<TArray<FEnhancedActionKeyMapping>&>(ConstMappings);

    // Mouse Wheel Zoom
    FEnhancedActionKeyMapping& MouseWheelUp = Mappings.AddDefaulted_GetRef();
    MouseWheelUp.Action = ZoomAction;
    MouseWheelUp.Key = EKeys::MouseWheelAxis;

    // Mouse XY for Rotate (with right-click hold)
    FEnhancedActionKeyMapping& MouseXY = Mappings.AddDefaulted_GetRef();
    MouseXY.Action = RotateAction;
    MouseXY.Key = EKeys::Mouse2D;

    // Xbox/Standard Gamepad Right Stick for Rotate
    FEnhancedActionKeyMapping& GamepadRotateX = Mappings.AddDefaulted_GetRef();
    GamepadRotateX.Action = RotateAction;
    GamepadRotateX.Key = EKeys::Gamepad_RightX;
    UInputModifierSwizzleAxis* GamepadXSwizzle = NewObject<UInputModifierSwizzleAxis>(NewMappingContext);
    GamepadXSwizzle->Order = EInputAxisSwizzle::YXZ;
    UInputModifierDeadZone* GamepadXDeadZone = NewObject<UInputModifierDeadZone>(NewMappingContext);
    GamepadXDeadZone->LowerThreshold = 0.2f;
    GamepadXDeadZone->UpperThreshold = 1.0f;
    GamepadRotateX.Modifiers.Add(GamepadXSwizzle);
    GamepadRotateX.Modifiers.Add(GamepadXDeadZone);

    FEnhancedActionKeyMapping& GamepadRotateY = Mappings.AddDefaulted_GetRef();
    GamepadRotateY.Action = RotateAction;
    GamepadRotateY.Key = EKeys::Gamepad_RightY;
    UInputModifierDeadZone* GamepadYDeadZone = NewObject<UInputModifierDeadZone>(NewMappingContext);
    GamepadYDeadZone->LowerThreshold = 0.2f;
    GamepadYDeadZone->UpperThreshold = 1.0f;
    GamepadRotateY.Modifiers.Add(GamepadYDeadZone);

    // PS5 Controller (GenericUSB) - Axis 3 for Zoom (with Negate)
    FEnhancedActionKeyMapping& PS5Zoom = Mappings.AddDefaulted_GetRef();
    PS5Zoom.Action = ZoomAction;
    PS5Zoom.Key = FKey(TEXT("GenericUSBController_Axis3"));
    UInputModifierNegate* PS5ZoomNegate = NewObject<UInputModifierNegate>(NewMappingContext);
    UInputModifierDeadZone* PS5ZoomDeadZone = NewObject<UInputModifierDeadZone>(NewMappingContext);
    PS5ZoomDeadZone->LowerThreshold = 0.2f;
    PS5ZoomDeadZone->UpperThreshold = 1.0f;
    PS5Zoom.Modifiers.Add(PS5ZoomNegate);
    PS5Zoom.Modifiers.Add(PS5ZoomDeadZone);

    // PS5 Controller (GenericUSB) - Axis 4 for Rotate (2D)
    FEnhancedActionKeyMapping& PS5Rotate = Mappings.AddDefaulted_GetRef();
    PS5Rotate.Action = RotateAction;
    PS5Rotate.Key = FKey(TEXT("GenericUSBController_Axis4"));
    UInputModifierDeadZone* PS5RotateDeadZone = NewObject<UInputModifierDeadZone>(NewMappingContext);
    PS5RotateDeadZone->LowerThreshold = 0.2f;
    PS5RotateDeadZone->UpperThreshold = 1.0f;
    PS5Rotate.Modifiers.Add(PS5RotateDeadZone);

    // Gamepad Triggers for Zoom
    FEnhancedActionKeyMapping& GamepadZoomIn = Mappings.AddDefaulted_GetRef();
    GamepadZoomIn.Action = ZoomAction;
    GamepadZoomIn.Key = EKeys::Gamepad_RightTriggerAxis;
    
    FEnhancedActionKeyMapping& GamepadZoomOut = Mappings.AddDefaulted_GetRef();
    GamepadZoomOut.Action = ZoomAction;
    GamepadZoomOut.Key = EKeys::Gamepad_LeftTriggerAxis;
    UInputModifierNegate* NegateLeft = NewObject<UInputModifierNegate>(NewMappingContext);
    GamepadZoomOut.Modifiers.Add(NegateLeft);

    // Mark the package and register the asset
    FAssetRegistryModule::AssetCreated(NewMappingContext);
    Package->MarkPackageDirty();
    Package->SetDirtyFlag(true);
    
    // Ensure the package is fully loaded
    Package->FullyLoad();

    // Configure save arguments
    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
    SaveArgs.Error = GError;
    SaveArgs.SaveFlags = SAVE_NoError;
    
    // Get the full file path
    FString PackageFilename = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());
    
    // Ensure directory exists
    FString Directory = FPaths::GetPath(PackageFilename);
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.DirectoryExists(*Directory))
    {
        PlatformFile.CreateDirectoryTree(*Directory);
    }
    
    // Save the package
    bool bSaved = UPackage::SavePackage(Package, NewMappingContext, *PackageFilename, SaveArgs);
    
    if (bSaved)
    {
        UE_LOG(LogCameraPawnCreation, Warning, TEXT("✓ Successfully saved input mapping context to %s"), *PackageFilename);
        
        // Force asset registry to update
        FAssetRegistryModule& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
        AssetRegistry.Get().AssetCreated(NewMappingContext);
        AssetRegistry.Get().SearchAllAssets(true);
    }
    else
    {
        UE_LOG(LogCameraPawnCreation, Error, TEXT("✗ Failed to save input mapping context to %s"), *PackageFilename);
        return false;
    }

    UE_LOG(LogCameraPawnCreation, Warning, TEXT("✓ Input mapping context created successfully"));
    return true;
#else
    UE_LOG(LogCameraPawnCreation, Error, TEXT("Input mapping context creation is only available in editor builds"));
    return false;
#endif
}

bool UCameraPawnCreationCommandlet::WriteSourceFile(const FString& FilePath, const FString& Content)
{
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    
    // Ensure directory exists
    FString Directory = FPaths::GetPath(FilePath);
    if (!PlatformFile.DirectoryExists(*Directory))
    {
        if (!PlatformFile.CreateDirectoryTree(*Directory))
        {
            UE_LOG(LogCameraPawnCreation, Error, TEXT("Failed to create directory: %s"), *Directory);
            return false;
        }
    }
    
    // Write file
    if (!FFileHelper::SaveStringToFile(Content, *FilePath))
    {
        UE_LOG(LogCameraPawnCreation, Error, TEXT("Failed to write file: %s"), *FilePath);
        return false;
    }
    
    UE_LOG(LogCameraPawnCreation, Log, TEXT("✓ Wrote file: %s"), *FilePath);
    return true;
}

void UCameraPawnCreationCommandlet::PrintUsage() const
{
    UE_LOG(LogCameraPawnCreation, Warning, TEXT(""));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("Usage: CameraPawnCreationCommandlet"));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT(""));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("Creates a camera pawn with spring arm and camera components,"));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("along with input actions for zoom and rotate."));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT(""));
}

void UCameraPawnCreationCommandlet::PrintSuccess() const
{
    UE_LOG(LogCameraPawnCreation, Warning, TEXT(""));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("✓ SUCCESS! Camera Pawn created successfully"));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT(""));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("Created Files:"));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("📄 Source/CharacterCreationCpp/CameraPawnCreationCommandlet/CameraPawn.h"));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("📄 Source/CharacterCreationCpp/CameraPawnCreationCommandlet/CameraPawn.cpp"));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT(""));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("Created Assets:"));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("🎮 /Game/Input/IA_Zoom (Axis1D)"));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("🎮 /Game/Input/IA_Rotate (Axis2D)"));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("🗺️ /Game/Input/IMC_CameraControl"));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT(""));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("Camera Settings:"));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("📏 Target Arm Length: 700 units"));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("🔄 Spring Arm Rotation: (0, -60, 0)"));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("📷 Camera Projection: Perspective"));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("🚫 Collision Test: Disabled"));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT(""));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("📋 Remember to rebuild the project to compile the new C++ classes!"));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT(""));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("ℹ️  Note: Generated code uses soft references (TSoftObjectPtr) for input assets"));
    UE_LOG(LogCameraPawnCreation, Warning, TEXT("   to prevent CDO constructor errors and improve loading performance."));
}