#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "Components/InputComponent.h"
#include "Engine/Engine.h"
#include "CharacterCreationLog.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "WarriorCharacter.generated.h"

UCLASS()
class CHARACTERCREATIONCPP_API AWarriorCharacter : public APaperCharacter
{
    GENERATED_BODY()

public:
    AWarriorCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    // Animation references - editable in Blueprint defaults
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animations")
    class UPaperFlipbook* IdleAnimation;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animations")
    class UPaperFlipbook* MoveAnimation;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animations")
    class UPaperFlipbook* AttackUpAnimation;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animations")
    class UPaperFlipbook* AttackDownAnimation;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animations")
    class UPaperFlipbook* AttackSideAnimation;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animations")
    class UPaperFlipbook* AttackUp2Animation;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animations")
    class UPaperFlipbook* AttackDown2Animation;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animations")
    class UPaperFlipbook* AttackSide2Animation;

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCameraComponent* CameraComponent;

    // Enhanced Input
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* AttackAction;

    // Movement variables
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float MovementSpeed;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float JumpVelocity;

    // State variables
    bool bIsMoving;
    bool bIsAttacking;
    bool bFacingRight;
    FTimerHandle AttackTimerHandle;

public:
    virtual void Tick(float DeltaTime) override;

    // Public animation accessors for Python
    UFUNCTION(BlueprintCallable, Category = "Animations")
    void SetIdleAnimation(UPaperFlipbook* Animation) { IdleAnimation = Animation; }
    
    UFUNCTION(BlueprintCallable, Category = "Animations")
    void SetMoveAnimation(UPaperFlipbook* Animation) { MoveAnimation = Animation; }
    
    UFUNCTION(BlueprintCallable, Category = "Animations")
    void SetAttackUpAnimation(UPaperFlipbook* Animation) { AttackUpAnimation = Animation; }
    
    UFUNCTION(BlueprintCallable, Category = "Animations")
    void SetAttackDownAnimation(UPaperFlipbook* Animation) { AttackDownAnimation = Animation; }
    
    UFUNCTION(BlueprintCallable, Category = "Animations")
    void SetAttackSideAnimation(UPaperFlipbook* Animation) { AttackSideAnimation = Animation; }
    
    UFUNCTION(BlueprintCallable, Category = "Animations")
    void SetAttackUp2Animation(UPaperFlipbook* Animation) { AttackUp2Animation = Animation; }
    
    UFUNCTION(BlueprintCallable, Category = "Animations")
    void SetAttackDown2Animation(UPaperFlipbook* Animation) { AttackDown2Animation = Animation; }
    
    UFUNCTION(BlueprintCallable, Category = "Animations")
    void SetAttackSide2Animation(UPaperFlipbook* Animation) { AttackSide2Animation = Animation; }

    // Public animation getters for Python
    UFUNCTION(BlueprintCallable, Category = "Animations")
    UPaperFlipbook* GetIdleAnimation() const { return IdleAnimation; }
    
    UFUNCTION(BlueprintCallable, Category = "Animations")
    UPaperFlipbook* GetMoveAnimation() const { return MoveAnimation; }
    
    UFUNCTION(BlueprintCallable, Category = "Animations")
    UPaperFlipbook* GetAttackUpAnimation() const { return AttackUpAnimation; }
    
    UFUNCTION(BlueprintCallable, Category = "Animations")
    UPaperFlipbook* GetAttackDownAnimation() const { return AttackDownAnimation; }
    
    UFUNCTION(BlueprintCallable, Category = "Animations")
    UPaperFlipbook* GetAttackSideAnimation() const { return AttackSideAnimation; }
    
    UFUNCTION(BlueprintCallable, Category = "Animations")
    UPaperFlipbook* GetAttackUp2Animation() const { return AttackUp2Animation; }
    
    UFUNCTION(BlueprintCallable, Category = "Animations")
    UPaperFlipbook* GetAttackDown2Animation() const { return AttackDown2Animation; }
    
    UFUNCTION(BlueprintCallable, Category = "Animations")
    UPaperFlipbook* GetAttackSide2Animation() const { return AttackSide2Animation; }

protected:
    // Enhanced Input functions
    void Move(const FInputActionValue& Value);
    void Attack(const FInputActionValue& Value);

    // Animation functions
    void SetAnimation(UPaperFlipbook* NewAnimation);
    void UpdateSpriteDirection();
    void EndAttack();
    void LoadAnimations();
    void LoadAndAssignAnimations();
    void TryLoadAnimationsForEditor();
    void TryLoadInputAssetsForEditor();

private:
    UPaperFlipbookComponent* SpriteComponent;
    float CurrentMoveRightValue;
    float CurrentMoveUpValue;
};