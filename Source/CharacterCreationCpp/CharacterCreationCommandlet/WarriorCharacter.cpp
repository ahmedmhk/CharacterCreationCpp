#include "WarriorCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PaperFlipbookComponent.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "HAL/IConsoleManager.h"

AWarriorCharacter::AWarriorCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Set default values
    MovementSpeed = 300.0f;
    JumpVelocity = 400.0f;
    bIsMoving = false;
    bIsAttacking = false;
    bFacingRight = true;
    CurrentMoveRightValue = 0.0f;
    CurrentMoveUpValue = 0.0f;

    // Configure character movement
    GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
    GetCharacterMovement()->JumpZVelocity = JumpVelocity;
    GetCharacterMovement()->GravityScale = 1.0f;
    GetCharacterMovement()->AirControl = 0.2f;
    GetCharacterMovement()->GroundFriction = 8.0f;
    
    // Constrain to 2D plane
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0, 1, 0));

    // Configure capsule collision
    GetCapsuleComponent()->SetCapsuleHalfHeight(50.0f);
    GetCapsuleComponent()->SetCapsuleRadius(25.0f);

    // Get sprite component reference
    SpriteComponent = GetSprite();
    if (SpriteComponent)
    {
        SpriteComponent->SetRelativeRotation(FRotator(0, 0, 0)); // Align vertically with capsule
    }

    // Create SpringArm component
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    SpringArmComponent->SetupAttachment(RootComponent);
    SpringArmComponent->TargetArmLength = 200.0f;
    SpringArmComponent->SetWorldRotation(FRotator(0.0f, -90.0f, 0.0f)); // Side view angle
    SpringArmComponent->bDoCollisionTest = false;
    SpringArmComponent->bInheritPitch = false;
    SpringArmComponent->bInheritYaw = false;
    SpringArmComponent->bInheritRoll = false;

    // Create Camera component
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
    CameraComponent->SetProjectionMode(ECameraProjectionMode::Perspective);
    CameraComponent->SetFieldOfView(90.0f);

    // NOTE: Asset loading moved to BeginPlay() to avoid LoadObject in constructor
    // This is safer and follows Unreal best practices
}

void AWarriorCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    // Setup Enhanced Input
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (PlayerController)
    {
        UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
        if (Subsystem)
        {
            // Use DefaultMappingContext that was loaded in constructor (if available)
            if (DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
                UE_LOG(LogCharacterCreation, Verbose, TEXT("✓ Added Input Mapping Context: IMC_PlayerInput"));
            }
            else
            {
                UE_LOG(LogCharacterCreation, Verbose, TEXT("✗ DefaultMappingContext is null - not loaded in constructor"));
            }
        }
    }

    // Input actions should already be loaded in constructor, log their status
    UE_LOG(LogCharacterCreation, Verbose, TEXT("BeginPlay - Input Actions Status:"));
    UE_LOG(LogCharacterCreation, Verbose, TEXT("MoveAction: %s"), MoveAction ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogCharacterCreation, Verbose, TEXT("AttackAction: %s"), AttackAction ? TEXT("YES") : TEXT("NO"));
    
    // Load animations in BeginPlay (safer than constructor loading)
    LoadAnimations();
    
    // Load input assets if not already set via Blueprint
    if (!DefaultMappingContext || !MoveAction || !AttackAction)
    {
        LoadInputAssets();
    }
    
    // Set initial animation on the sprite component
    if (SpriteComponent && IdleAnimation)
    {
        UE_LOG(LogCharacterCreation, Verbose, TEXT("Setting initial IdleAnimation on sprite component"));
        SpriteComponent->SetFlipbook(IdleAnimation);
    }
    else
    {
        UE_LOG(LogCharacterCreation, Verbose, TEXT("Cannot set initial animation - SpriteComponent: %s, IdleAnimation: %s"), 
            SpriteComponent ? TEXT("Valid") : TEXT("NULL"),
            IdleAnimation ? TEXT("Valid") : TEXT("NULL"));
    }
}

void AWarriorCharacter::LoadAnimations()
{
    UE_LOG(LogCharacterCreation, Verbose, TEXT("=== LoadAnimations() called - Loading and assigning all 8 animations ==="));
    
    // Load and assign primary animation assets directly to UPROPERTY variables
    IdleAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/Idle"));
    MoveAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/Move"));
    AttackUpAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackUpwards"));
    AttackDownAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackDownwards"));
    AttackSideAnimation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackSideways"));
    
    // Load and assign secondary attack animation variants
    AttackUp2Animation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackUpwards2"));
    AttackDown2Animation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackDownwards2"));
    AttackSide2Animation = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/AttackSideways2"));

    // Log assignment status
    UE_LOG(LogCharacterCreation, Verbose, TEXT("IdleAnimation assigned: %s"), IdleAnimation ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogCharacterCreation, Verbose, TEXT("MoveAnimation assigned: %s"), MoveAnimation ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogCharacterCreation, Verbose, TEXT("AttackUpAnimation assigned: %s"), AttackUpAnimation ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogCharacterCreation, Verbose, TEXT("AttackDownAnimation assigned: %s"), AttackDownAnimation ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogCharacterCreation, Verbose, TEXT("AttackSideAnimation assigned: %s"), AttackSideAnimation ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogCharacterCreation, Verbose, TEXT("AttackUp2Animation assigned: %s"), AttackUp2Animation ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogCharacterCreation, Verbose, TEXT("AttackDown2Animation assigned: %s"), AttackDown2Animation ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogCharacterCreation, Verbose, TEXT("AttackSide2Animation assigned: %s"), AttackSide2Animation ? TEXT("YES") : TEXT("NO"));
    
    // Count successful assignments
    int32 AssignedCount = 0;
    if (IdleAnimation) AssignedCount++;
    if (MoveAnimation) AssignedCount++;
    if (AttackUpAnimation) AssignedCount++;
    if (AttackDownAnimation) AssignedCount++;
    if (AttackSideAnimation) AssignedCount++;
    if (AttackUp2Animation) AssignedCount++;
    if (AttackDown2Animation) AssignedCount++;
    if (AttackSide2Animation) AssignedCount++;
    
    UE_LOG(LogCharacterCreation, Verbose, TEXT("=== Animation Assignment Complete: %d/8 animations assigned ==="), AssignedCount);
    
    if (GEngine)
    {
        if (IdleAnimation) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("✓ Assigned Idle Animation"));
        if (MoveAnimation) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("✓ Assigned Move Animation"));
        if (AttackUpAnimation) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("✓ Assigned AttackUp Animation"));
        if (AttackDownAnimation) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("✓ Assigned AttackDown Animation"));
        if (AttackSideAnimation) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("✓ Assigned AttackSide Animation"));
        if (AttackUp2Animation) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("✓ Assigned AttackUp2 Animation"));
        if (AttackDown2Animation) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("✓ Assigned AttackDown2 Animation"));
        if (AttackSide2Animation) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("✓ Assigned AttackSide2 Animation"));
        
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Animation Assignment: %d/8 Complete"), AssignedCount));
    }
}

// Removed legacy LoadAndAssignAnimations function - no longer needed

void AWarriorCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Cast to Enhanced Input Component
    UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

    // Bind Move Action (WASD)
    if (MoveAction)
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWarriorCharacter::Move);
        UE_LOG(LogCharacterCreation, Verbose, TEXT("✓ Bound Move Action"));
    }
    else
    {
        UE_LOG(LogCharacterCreation, Verbose, TEXT("✗ MoveAction is null, cannot bind"));
    }

    // Bind Attack Action (Left Mouse)
    if (AttackAction)
    {
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AWarriorCharacter::Attack);
        UE_LOG(LogCharacterCreation, Verbose, TEXT("✓ Bound Attack Action"));
    }
    else
    {
        UE_LOG(LogCharacterCreation, Verbose, TEXT("✗ AttackAction is null, cannot bind"));
    }
}

// Enhanced Input Functions
void AWarriorCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();
    
    CurrentMoveRightValue = MovementVector.X;
    CurrentMoveUpValue = MovementVector.Y;
    
    UE_LOG(LogCharacterCreation, Log, TEXT("Move called with value: (%f, %f)"), MovementVector.X, MovementVector.Y);
    
    if (FMath::Abs(MovementVector.X) > 0.1f)
    {
        // Add horizontal movement input
        AddMovementInput(FVector(1, 0, 0), MovementVector.X);
        
        // Update facing direction
        if (MovementVector.X > 0)
        {
            bFacingRight = true;
        }
        else
        {
            bFacingRight = false;
        }
    }
    
    if (FMath::Abs(MovementVector.Y) > 0.1f)
    {
        // Add vertical movement input for platforming
        AddMovementInput(FVector(0, 0, 1), MovementVector.Y);
    }
}

void AWarriorCharacter::Attack(const FInputActionValue& Value)
{
    if (!bIsAttacking && AttackSideAnimation)
    {
        bIsAttacking = true;
        SetAnimation(AttackSideAnimation);
        
        // Set timer to end attack
        GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AWarriorCharacter::EndAttack, 0.5f, false);
        
        UE_LOG(LogCharacterCreation, Verbose, TEXT("Attack performed!"));
        
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Attack!"));
        }
    }
}

void AWarriorCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update movement state
    bIsMoving = (FMath::Abs(CurrentMoveRightValue) > 0.1f || FMath::Abs(CurrentMoveUpValue) > 0.1f);

    // Update animation based on state
    if (!bIsAttacking)
    {
        if (bIsMoving && MoveAnimation)
        {
            SetAnimation(MoveAnimation);
        }
        else if (!bIsMoving && IdleAnimation)
        {
            SetAnimation(IdleAnimation);
        }
    }

    // Update sprite direction
    UpdateSpriteDirection();
}



void AWarriorCharacter::SetAnimation(UPaperFlipbook* NewAnimation)
{
    if (SpriteComponent && NewAnimation)
    {
        UE_LOG(LogCharacterCreation, Verbose, TEXT("SetAnimation called with: %s"), *NewAnimation->GetName());
        SpriteComponent->SetFlipbook(NewAnimation);
    }
    else
    {
        UE_LOG(LogCharacterCreation, Error, TEXT("SetAnimation failed - SpriteComponent: %s, NewAnimation: %s"), 
            SpriteComponent ? TEXT("Valid") : TEXT("Invalid"),
            NewAnimation ? TEXT("Valid") : TEXT("Invalid"));
    }
}

void AWarriorCharacter::UpdateSpriteDirection()
{
    if (SpriteComponent)
    {
        // Flip sprite based on facing direction
        FVector Scale = SpriteComponent->GetRelativeScale3D();
        if (bFacingRight)
        {
            Scale.X = FMath::Abs(Scale.X);
        }
        else
        {
            Scale.X = -FMath::Abs(Scale.X);
        }
        SpriteComponent->SetRelativeScale3D(Scale);
    }
}


void AWarriorCharacter::EndAttack()
{
    bIsAttacking = false;
    
    // Return to appropriate animation
    if (bIsMoving && MoveAnimation)
    {
        SetAnimation(MoveAnimation);
    }
    else if (IdleAnimation)
    {
        SetAnimation(IdleAnimation);
    }
}

// TryLoadAnimationsForEditor() removed - LoadObject should not be called in constructors

void AWarriorCharacter::LoadInputAssets()
{
    UE_LOG(LogCharacterCreation, Verbose, TEXT("=== LoadInputAssets() called in BeginPlay ==="));
    
    // Load input assets safely in BeginPlay (not in constructor)
    if (!DefaultMappingContext)
    {
        DefaultMappingContext = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/Input/IMC_PlayerInput"));
    }
    if (!MoveAction)
    {
        MoveAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/IA_Move"));
    }
    if (!AttackAction)
    {
        AttackAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/IA_Attack"));
    }
    
    // Log what was loaded
    UE_LOG(LogCharacterCreation, Verbose, TEXT("Loaded input assets:"));
    UE_LOG(LogCharacterCreation, Verbose, TEXT("- DefaultMappingContext: %s"), DefaultMappingContext ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogCharacterCreation, Verbose, TEXT("- MoveAction: %s"), MoveAction ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogCharacterCreation, Verbose, TEXT("- AttackAction: %s"), AttackAction ? TEXT("YES") : TEXT("NO"));
}