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

    // Configure capsule collision to fit sprite better
    // Typical sprite character is about 32x32 pixels, adjust capsule accordingly
    GetCapsuleComponent()->SetCapsuleHalfHeight(32.0f);
    GetCapsuleComponent()->SetCapsuleRadius(16.0f);

    // Get sprite component reference
    SpriteComponent = GetSprite();
    UE_LOG(LogCharacterCreation, Warning, TEXT("=== %s CONSTRUCTOR: SpriteComponent = %s ==="), 
        *GetClass()->GetName(), SpriteComponent ? TEXT("VALID") : TEXT("NULL"));
    
    if (SpriteComponent)
    {
        SpriteComponent->SetRelativeRotation(FRotator(0, 0, 0)); // Align vertically with capsule
        // Offset sprite down so feet align with capsule bottom
        // Capsule half-height is 32, so offset sprite down by ~28 units to align feet
        SpriteComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -28.0f));
        
        // Try loading animations right here in constructor for the specific derived class
        // This is a workaround for virtual functions not working in constructors
        FString ClassName = GetClass()->GetName();
        if (ClassName.Contains(TEXT("Purple")))
        {
            UE_LOG(LogCharacterCreation, Warning, TEXT("Loading Purple animations in constructor"));
            UPaperFlipbook* TempIdle = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/Idle_Warrior_Purple"));
            if (TempIdle)
            {
                SpriteComponent->SetFlipbook(TempIdle);
                UE_LOG(LogCharacterCreation, Warning, TEXT("✓✓✓ Set Purple idle animation in CONSTRUCTOR ✓✓✓"));
            }
        }
        else if (ClassName.Contains(TEXT("Blue")))
        {
            UE_LOG(LogCharacterCreation, Warning, TEXT("Loading Blue animations in constructor"));
            UPaperFlipbook* TempIdle = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/Idle_Warrior_Blue"));
            if (TempIdle)
            {
                SpriteComponent->SetFlipbook(TempIdle);
                UE_LOG(LogCharacterCreation, Warning, TEXT("✓✓✓ Set Blue idle animation in CONSTRUCTOR ✓✓✓"));
            }
        }
        else if (ClassName.Contains(TEXT("Red")))
        {
            UE_LOG(LogCharacterCreation, Warning, TEXT("Loading Red animations in constructor"));
            UPaperFlipbook* TempIdle = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/Idle_Warrior_Red"));
            if (TempIdle)
            {
                SpriteComponent->SetFlipbook(TempIdle);
                UE_LOG(LogCharacterCreation, Warning, TEXT("✓✓✓ Set Red idle animation in CONSTRUCTOR ✓✓✓"));
            }
        }
    }
    else
    {
        UE_LOG(LogCharacterCreation, Error, TEXT("SpriteComponent is NULL in constructor!"));
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

    // Set default input assets as UPROPERTY values for Blueprint editor
    TryLoadInputAssetsForEditor();
}

void AWarriorCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    
    // Load animations after object is fully constructed - virtual functions work correctly now
    UE_LOG(LogCharacterCreation, Warning, TEXT("=== %s::PostInitializeComponents() CALLED ==="), *GetClass()->GetName());
    
    // Verify sprite component exists
    if (!SpriteComponent)
    {
        UE_LOG(LogCharacterCreation, Error, TEXT("SpriteComponent is NULL in PostInitializeComponents!"));
        return;
    }
    
    UE_LOG(LogCharacterCreation, Warning, TEXT("Calling LoadAnimations() for %s"), *GetClass()->GetName());
    LoadAnimations();
    
    // Count successful loads
    int32 LoadedCount = 0;
    if (IdleAnimation) LoadedCount++;
    if (MoveAnimation) LoadedCount++;
    if (AttackUpAnimation) LoadedCount++;
    if (AttackDownAnimation) LoadedCount++;
    if (AttackSideAnimation) LoadedCount++;
    if (AttackUp2Animation) LoadedCount++;
    if (AttackDown2Animation) LoadedCount++;
    if (AttackSide2Animation) LoadedCount++;
    
    UE_LOG(LogCharacterCreation, Warning, TEXT("PostInitializeComponents loaded %d/8 animations"), LoadedCount);
    
    // Don't set the flipbook here - let the derived class do it in LoadAnimations()
    // This avoids conflicts between base and derived class both trying to set it
    if (!IdleAnimation)
    {
        UE_LOG(LogCharacterCreation, Error, TEXT("No IdleAnimation loaded for %s!"), *GetClass()->GetName());
    }
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
    
    // Animations should already be loaded in PostInitializeComponents
    UE_LOG(LogCharacterCreation, Verbose, TEXT("BeginPlay - Animations already loaded in PostInitializeComponents (%d/8)"), 
        (IdleAnimation ? 1 : 0) + (MoveAnimation ? 1 : 0) + (AttackUpAnimation ? 1 : 0) + 
        (AttackDownAnimation ? 1 : 0) + (AttackSideAnimation ? 1 : 0) + (AttackUp2Animation ? 1 : 0) + 
        (AttackDown2Animation ? 1 : 0) + (AttackSide2Animation ? 1 : 0));
    
    // Load input assets if not already set via Blueprint
    if (!DefaultMappingContext || !MoveAction || !AttackAction)
    {
        LoadInputAssets();
    }
    
    // Animation should already be set in PostInitializeComponents, just verify
    if (SpriteComponent && IdleAnimation)
    {
        UE_LOG(LogCharacterCreation, Verbose, TEXT("✓ Animation already set in PostInitializeComponents"));
    }
    else
    {
        UE_LOG(LogCharacterCreation, Warning, TEXT("⚠️ Animation not properly set - SpriteComponent: %s, IdleAnimation: %s"), 
            SpriteComponent ? TEXT("Valid") : TEXT("NULL"),
            IdleAnimation ? TEXT("Valid") : TEXT("NULL"));
    }
}

void AWarriorCharacter::LoadAnimations()
{
    // Base implementation - derived classes should override this to load their specific animations
    UE_LOG(LogCharacterCreation, Error, TEXT("=== BASE CLASS LoadAnimations() called - This should NOT happen! Derived class override should be called instead! ==="));
    
    // This is intentionally empty - derived classes like WarriorPurpleCharacter, WarriorBlueCharacter, etc.
    // will override this method to load their character-specific animations
}

void AWarriorCharacter::LoadAndAssignAnimations()
{
    // Legacy function - now just calls LoadAnimations for backward compatibility
    LoadAnimations();
}

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


void AWarriorCharacter::TryLoadInputAssetsForEditor()
{
    UE_LOG(LogCharacterCreation, Verbose, TEXT("=== TryLoadInputAssetsForEditor() called in constructor ==="));
    
    // Try to load input assets for editor display (will silently fail if assets don't exist yet)
    DefaultMappingContext = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/Input/IMC_PlayerInput"));
    MoveAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/IA_Move"));
    AttackAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/IA_Attack"));
    
    // Log what was loaded
    UE_LOG(LogCharacterCreation, Verbose, TEXT("Constructor loaded input assets:"));
    UE_LOG(LogCharacterCreation, Verbose, TEXT("- DefaultMappingContext: %s"), DefaultMappingContext ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogCharacterCreation, Verbose, TEXT("- MoveAction: %s"), MoveAction ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogCharacterCreation, Verbose, TEXT("- AttackAction: %s"), AttackAction ? TEXT("YES") : TEXT("NO"));
}