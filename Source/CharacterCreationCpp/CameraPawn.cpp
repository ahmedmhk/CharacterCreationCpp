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
    PrimaryActorTick.bCanEverTick = false;

    // Create root component
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // Create spring arm component
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComponent->SetupAttachment(RootComponent);
    SpringArmComponent->TargetArmLength = 700.0f;
    SpringArmComponent->SetRelativeRotation(FRotator(0.0f, -60.0f, 0.0f));
    SpringArmComponent->bDoCollisionTest = false;
    SpringArmComponent->bUsePawnControlRotation = true;

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
    ZoomSpeed = 50.0f;
    MinZoomDistance = 200.0f;
    MaxZoomDistance = 2000.0f;
    RotationSpeed = 100.0f;

    // Load input assets
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

void ACameraPawn::BeginPlay()
{
    Super::BeginPlay();

    // Add input mapping context
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            if (CameraMappingContext)
            {
                Subsystem->AddMappingContext(CameraMappingContext, 0);
            }
        }
    }
}

void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Bind zoom action
        if (ZoomAction)
        {
            EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ACameraPawn::Zoom);
        }

        // Bind rotate action
        if (RotateAction)
        {
            EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this, &ACameraPawn::Rotate);
        }
    }
}

void ACameraPawn::Zoom(const FInputActionValue& Value)
{
    const float ZoomValue = Value.Get<float>();
    
    if (SpringArmComponent)
    {
        float NewTargetArmLength = SpringArmComponent->TargetArmLength - (ZoomValue * ZoomSpeed);
        SpringArmComponent->TargetArmLength = FMath::Clamp(NewTargetArmLength, MinZoomDistance, MaxZoomDistance);
    }
}

void ACameraPawn::Rotate(const FInputActionValue& Value)
{
    const FVector2D RotationValue = Value.Get<FVector2D>();
    
    if (Controller)
    {
        // Add yaw (horizontal) rotation
        AddControllerYawInput(RotationValue.X * RotationSpeed * GetWorld()->GetDeltaSeconds());
        
        // Add pitch (vertical) rotation
        AddControllerPitchInput(-RotationValue.Y * RotationSpeed * GetWorld()->GetDeltaSeconds());
    }
}
