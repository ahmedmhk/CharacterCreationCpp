#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "Components/InputComponent.h"
#include "Engine/Engine.h"
#include "WarriorCharacter.generated.h"

UCLASS()
class PAPERFLIPBOOKPROJECT_API AWarriorCharacter : public APaperCharacter
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
    // Input functions
    void MoveRight(float Value);
    void MoveUp(float Value);
    void Jump();
    void AttackUp();
    void AttackDown();
    void AttackLeft();
    void AttackRight();
    void Attack();
    void AttackUp2();
    void AttackDown2();
    void AttackLeft2();
    void AttackRight2();

    // Animation functions
    void SetAnimation(UPaperFlipbook* NewAnimation);
    void UpdateSpriteDirection();
    void EndAttack();
    void LoadAnimations();
    void LoadAndAssignAnimations();

private:
    UPaperFlipbookComponent* SpriteComponent;
    float CurrentMoveRightValue;
    float CurrentMoveUpValue;
};