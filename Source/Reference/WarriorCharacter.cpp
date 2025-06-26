#include "WarriorCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PaperFlipbookComponent.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

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
        SpriteComponent->SetRelativeRotation(FRotator(0, 0, -90)); // Rotate for side-view
    }

    // Automatically assign all flipbook animations in constructor
    LoadAndAssignAnimations();
}

void AWarriorCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    // Only load animations if they're not already assigned (for backward compatibility)
    if (!IdleAnimation)
    {
        LoadAnimations();
    }
    
    // Set initial animation
    if (IdleAnimation)
    {
        SetAnimation(IdleAnimation);
    }
}

void AWarriorCharacter::LoadAndAssignAnimations()
{
    UE_LOG(LogTemp, Warning, TEXT("=== LoadAndAssignAnimations() called - Loading and assigning all 8 animations ==="));
    
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
    UE_LOG(LogTemp, Warning, TEXT("IdleAnimation assigned: %s"), IdleAnimation ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("MoveAnimation assigned: %s"), MoveAnimation ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("AttackUpAnimation assigned: %s"), AttackUpAnimation ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("AttackDownAnimation assigned: %s"), AttackDownAnimation ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("AttackSideAnimation assigned: %s"), AttackSideAnimation ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("AttackUp2Animation assigned: %s"), AttackUp2Animation ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("AttackDown2Animation assigned: %s"), AttackDown2Animation ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("AttackSide2Animation assigned: %s"), AttackSide2Animation ? TEXT("YES") : TEXT("NO"));
    
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
    
    UE_LOG(LogTemp, Warning, TEXT("=== Animation Assignment Complete: %d/8 animations assigned ==="), AssignedCount);
    
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

void AWarriorCharacter::LoadAnimations()
{
    // Legacy function - now just calls LoadAndAssignAnimations for backward compatibility
    LoadAndAssignAnimations();
}

void AWarriorCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Bind axis mappings
    PlayerInputComponent->BindAxis("MoveRight", this, &AWarriorCharacter::MoveRight);
    PlayerInputComponent->BindAxis("MoveUp", this, &AWarriorCharacter::MoveUp);

    // Bind action mappings
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AWarriorCharacter::Jump);
    PlayerInputComponent->BindAction("AttackUp", IE_Pressed, this, &AWarriorCharacter::AttackUp);
    PlayerInputComponent->BindAction("AttackDown", IE_Pressed, this, &AWarriorCharacter::AttackDown);
    PlayerInputComponent->BindAction("AttackLeft", IE_Pressed, this, &AWarriorCharacter::AttackLeft);
    PlayerInputComponent->BindAction("AttackRight", IE_Pressed, this, &AWarriorCharacter::AttackRight);
    PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AWarriorCharacter::Attack);
    PlayerInputComponent->BindAction("AttackUp2", IE_Pressed, this, &AWarriorCharacter::AttackUp2);
    PlayerInputComponent->BindAction("AttackDown2", IE_Pressed, this, &AWarriorCharacter::AttackDown2);
    PlayerInputComponent->BindAction("AttackLeft2", IE_Pressed, this, &AWarriorCharacter::AttackLeft2);
    PlayerInputComponent->BindAction("AttackRight2", IE_Pressed, this, &AWarriorCharacter::AttackRight2);
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
            UE_LOG(LogTemp, Warning, TEXT("Tick: Setting Move animation (bIsMoving=true)"));
            SetAnimation(MoveAnimation);
        }
        else if (!bIsMoving && IdleAnimation)
        {
            UE_LOG(LogTemp, Warning, TEXT("Tick: Setting Idle animation (bIsMoving=false)"));
            SetAnimation(IdleAnimation);
        }
    }

    // Update sprite direction
    UpdateSpriteDirection();
}

void AWarriorCharacter::MoveRight(float Value)
{
    CurrentMoveRightValue = Value;
    UE_LOG(LogTemp, Warning, TEXT("MoveRight called with value: %f"), Value);
    
    if (FMath::Abs(Value) > 0.1f)
    {
        // Add movement input
        AddMovementInput(FVector(1, 0, 0), Value);
        
        // Update facing direction
        if (Value > 0)
        {
            bFacingRight = true;
        }
        else
        {
            bFacingRight = false;
        }
        
        UE_LOG(LogTemp, Warning, TEXT("Movement input added, bFacingRight: %s"), bFacingRight ? TEXT("true") : TEXT("false"));
    }
}

void AWarriorCharacter::MoveUp(float Value)
{
    CurrentMoveUpValue = Value;
    
    if (FMath::Abs(Value) > 0.1f)
    {
        // Add movement input for vertical movement (platforming)
        AddMovementInput(FVector(0, 0, 1), Value);
    }
}

void AWarriorCharacter::Jump()
{
    ACharacter::Jump();
}

void AWarriorCharacter::AttackUp()
{
    if (!bIsAttacking && AttackUpAnimation)
    {
        bIsAttacking = true;
        SetAnimation(AttackUpAnimation);
        
        // Set timer to end attack
        GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AWarriorCharacter::EndAttack, 0.5f, false);
        
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Attack Up!"));
        }
    }
}

void AWarriorCharacter::AttackDown()
{
    if (!bIsAttacking && AttackDownAnimation)
    {
        bIsAttacking = true;
        SetAnimation(AttackDownAnimation);
        
        GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AWarriorCharacter::EndAttack, 0.5f, false);
        
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Attack Down!"));
        }
    }
}

void AWarriorCharacter::AttackLeft()
{
    if (!bIsAttacking && AttackSideAnimation)
    {
        bIsAttacking = true;
        bFacingRight = false;
        SetAnimation(AttackSideAnimation);
        
        GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AWarriorCharacter::EndAttack, 0.5f, false);
        
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Attack Left!"));
        }
    }
}

void AWarriorCharacter::AttackRight()
{
    if (!bIsAttacking && AttackSideAnimation)
    {
        bIsAttacking = true;
        bFacingRight = true;
        SetAnimation(AttackSideAnimation);
        
        GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AWarriorCharacter::EndAttack, 0.5f, false);
        
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Attack Right!"));
        }
    }
}

void AWarriorCharacter::Attack()
{
    if (!bIsAttacking && AttackSideAnimation)
    {
        bIsAttacking = true;
        SetAnimation(AttackSideAnimation);
        
        GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AWarriorCharacter::EndAttack, 0.5f, false);
        
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Mouse Attack!"));
        }
    }
}

void AWarriorCharacter::SetAnimation(UPaperFlipbook* NewAnimation)
{
    if (SpriteComponent && NewAnimation)
    {
        UE_LOG(LogTemp, Warning, TEXT("SetAnimation called with: %s"), *NewAnimation->GetName());
        SpriteComponent->SetFlipbook(NewAnimation);
        UE_LOG(LogTemp, Warning, TEXT("Animation set successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SetAnimation failed - SpriteComponent: %s, NewAnimation: %s"), 
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

void AWarriorCharacter::AttackUp2()
{
    if (!bIsAttacking && AttackUp2Animation)
    {
        bIsAttacking = true;
        SetAnimation(AttackUp2Animation);
        
        GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AWarriorCharacter::EndAttack, 0.5f, false);
        
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, TEXT("Attack Up 2!"));
        }
    }
}

void AWarriorCharacter::AttackDown2()
{
    if (!bIsAttacking && AttackDown2Animation)
    {
        bIsAttacking = true;
        SetAnimation(AttackDown2Animation);
        
        GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AWarriorCharacter::EndAttack, 0.5f, false);
        
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, TEXT("Attack Down 2!"));
        }
    }
}

void AWarriorCharacter::AttackLeft2()
{
    if (!bIsAttacking && AttackSide2Animation)
    {
        bIsAttacking = true;
        bFacingRight = false;
        SetAnimation(AttackSide2Animation);
        
        GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AWarriorCharacter::EndAttack, 0.5f, false);
        
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, TEXT("Attack Left 2!"));
        }
    }
}

void AWarriorCharacter::AttackRight2()
{
    if (!bIsAttacking && AttackSide2Animation)
    {
        bIsAttacking = true;
        bFacingRight = true;
        SetAnimation(AttackSide2Animation);
        
        GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AWarriorCharacter::EndAttack, 0.5f, false);
        
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, TEXT("Attack Right 2!"));
        }
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