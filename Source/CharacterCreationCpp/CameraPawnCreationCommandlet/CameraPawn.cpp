#include "CameraPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"

ACameraPawn::ACameraPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create root component
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // Create spring arm component
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComponent->SetupAttachment(RootComponent);
    SpringArmComponent->TargetArmLength = 700.0f;
    SpringArmComponent->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));
    SpringArmComponent->bDoCollisionTest = false;
    SpringArmComponent->bUsePawnControlRotation = false;

    // Create camera component
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
    CameraComponent->SetProjectionMode(ECameraProjectionMode::Perspective);
    CameraComponent->SetFieldOfView(90.0f);

    // Create floating pawn movement for basic movement
    FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
    FloatingPawnMovement->MaxSpeed = 600.0f;
    FloatingPawnMovement->Acceleration = 4000.0f;
    FloatingPawnMovement->Deceleration = 8000.0f;

    // Set default camera settings
    ZoomSpeed = 100.0f;  // Increased for more responsive mouse wheel zooming
    MinZoomDistance = 200.0f;
    MaxZoomDistance = 2000.0f;
    RotationSpeed = 100.0f;
    
    // Initialize target values
    TargetArmLength = 700.0f;
    TargetRotation = FRotator(-60.0f, 0.0f, 0.0f);
    ZoomInputValue = 0.0f;
    RotationInputValue = FVector2D::ZeroVector;

    // Load input assets directly in constructor
    static ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContextFinder(TEXT("/Game/Input/IMC_CameraControl"));
    if (MappingContextFinder.Succeeded())
    {
        CameraMappingContext = MappingContextFinder.Object;
    }
    
    static ConstructorHelpers::FObjectFinder<UInputAction> ZoomActionFinder(TEXT("/Game/Input/IA_Zoom"));
    if (ZoomActionFinder.Succeeded())
    {
        ZoomAction = ZoomActionFinder.Object;
    }
    
    static ConstructorHelpers::FObjectFinder<UInputAction> RotateActionFinder(TEXT("/Game/Input/IA_Rotate"));
    if (RotateActionFinder.Succeeded())
    {
        RotateAction = RotateActionFinder.Object;
    }
}

void ACameraPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (SpringArmComponent)
    {
        // Smoothly interpolate zoom
        float CurrentArmLength = SpringArmComponent->TargetArmLength;
        float NewArmLength = FMath::FInterpTo(CurrentArmLength, TargetArmLength, DeltaTime, ZoomSmoothSpeed);
        SpringArmComponent->TargetArmLength = NewArmLength;
        
        // Smoothly interpolate rotation
        FRotator CurrentRotation = SpringArmComponent->GetRelativeRotation();
        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSmoothSpeed);
        SpringArmComponent->SetRelativeRotation(NewRotation);
    }
}

void ACameraPawn::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize target values from current spring arm settings
    if (SpringArmComponent)
    {
        TargetArmLength = SpringArmComponent->TargetArmLength;
        TargetRotation = SpringArmComponent->GetRelativeRotation();
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("CameraPawn::BeginPlay() - Starting input setup"));
    }

    // Add input mapping context
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("CameraPawn - Found PlayerController"));
        }

        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("CameraPawn - Found Enhanced Input Subsystem"));
            }

            // Load and add mapping context if it exists
            if (CameraMappingContext)
            {
                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, 
                        TEXT("CameraPawn - InputMappingContext loaded successfully"));
                }

                // Use the directly loaded context
                if (CameraMappingContext)
                {
                    Subsystem->AddMappingContext(CameraMappingContext, 0);
                    if (GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, 
                            FString::Printf(TEXT("CameraPawn - Successfully added IMC: %s"), *CameraMappingContext->GetName()));
                    }
                }
                else
                {
                    if (GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("CameraPawn - FAILED to load InputMappingContext!"));
                    }
                }
            }
            else
            {
                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("CameraPawn - CameraMappingContext is NULL!"));
                }
            }
        }
        else
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("CameraPawn - Failed to get Enhanced Input Subsystem!"));
            }
        }
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("CameraPawn - No PlayerController found!"));
        }
    }
}

void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("CameraPawn::SetupPlayerInputComponent() - Setting up input bindings"));
    }

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("CameraPawn - Got EnhancedInputComponent"));
        }

        // Bind zoom action if it exists
        if (ZoomAction)
        {
            EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ACameraPawn::Zoom);
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, 
                    FString::Printf(TEXT("CameraPawn - Successfully bound Zoom Action: %s"), *ZoomAction->GetName()));
            }
        }
        else
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("CameraPawn - ZoomAction is NULL!"));
            }
        }

        // Bind rotate action if it exists
        if (RotateAction)
        {
            EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this, &ACameraPawn::Rotate);
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, 
                    FString::Printf(TEXT("CameraPawn - Successfully bound Rotate Action: %s"), *RotateAction->GetName()));
            }
        }
        else
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("CameraPawn - RotateAction is NULL!"));
            }
        }
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("CameraPawn - Failed to get EnhancedInputComponent!"));
        }
    }
}

void ACameraPawn::Zoom(const FInputActionValue& Value)
{
    const float ZoomValue = Value.Get<float>();
    ZoomInputValue = ZoomValue;
    
    // Determine if input is from controller (continuous) or mouse wheel (discrete)
    // Controller input is typically in range -1 to 1, mouse wheel is usually larger discrete values
    float ZoomStep = 0.0f;
    
    if (FMath::Abs(ZoomValue) <= 1.0f)
    {
        // Controller input: scale to 2000 units per second at full deflection
        ZoomStep = ZoomValue * 2000.0f * GetWorld()->GetDeltaSeconds();
    }
    else
    {
        // Mouse wheel input: use full zoom speed for discrete steps
        ZoomStep = ZoomValue * ZoomSpeed;
    }
    
    TargetArmLength = FMath::Clamp(TargetArmLength - ZoomStep, MinZoomDistance, MaxZoomDistance);
    
    // Debug message to check if zoom input is being received
    if (GEngine && FMath::Abs(ZoomValue) > 0.01f)
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, 
            FString::Printf(TEXT("Zoom Input: %f | Target: %.1f | Step: %.1f"), ZoomValue, TargetArmLength, ZoomStep));
    }
}

void ACameraPawn::Rotate(const FInputActionValue& Value)
{
    const FVector2D RotationValue = Value.Get<FVector2D>();
    RotationInputValue = RotationValue;
    
    // Update target rotation based on input
    float DeltaTime = GetWorld()->GetDeltaSeconds();
    
    // Apply yaw (horizontal) rotation - inverted for natural camera control
    TargetRotation.Yaw += RotationValue.X * RotationSpeed * DeltaTime;
    
    // Apply pitch (vertical) rotation with clamping - inverted for natural camera control
    float NewPitch = TargetRotation.Pitch + (RotationValue.Y * RotationSpeed * DeltaTime);
    TargetRotation.Pitch = FMath::Clamp(NewPitch, -80.0f, -10.0f); // Limit vertical rotation
    
    // Debug message for rotation input
    if (GEngine && RotationValue.Size() > 0.01f)
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, 
            FString::Printf(TEXT("Rotation Input: X=%.2f Y=%.2f | Target: P=%.1f Y=%.1f"), 
                RotationValue.X, RotationValue.Y, TargetRotation.Pitch, TargetRotation.Yaw));
    }
}
