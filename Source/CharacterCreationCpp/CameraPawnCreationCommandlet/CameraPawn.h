#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "CameraPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UFloatingPawnMovement;
class UInputMappingContext;
class UInputAction;

UCLASS()
class CHARACTERCREATIONCPP_API ACameraPawn : public APawn
{
    GENERATED_BODY()

public:
    ACameraPawn();
    virtual void Tick(float DeltaTime) override;
    
    // Getter for camera component (used for billboard effect)
    UFUNCTION(BlueprintCallable, Category = "Camera")
    UCameraComponent* GetCameraComponent() const { return CameraComponent; }

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* CameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    UFloatingPawnMovement* FloatingPawnMovement;

    // Input - Using direct references for reliability
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* CameraMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* ZoomAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* RotateAction;

    // Input callbacks
    void Zoom(const FInputActionValue& Value);
    void Rotate(const FInputActionValue& Value);

    // Camera settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float ZoomSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float MinZoomDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float MaxZoomDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float RotationSpeed;

    // Smooth interpolation
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (ClampMin = "0.1", ClampMax = "20.0"))
    float ZoomSmoothSpeed = 8.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (ClampMin = "0.1", ClampMax = "20.0"))
    float RotationSmoothSpeed = 10.0f;

private:
    // Target values for smooth interpolation
    float TargetArmLength;
    FRotator TargetRotation;
    float ZoomInputValue;
    FVector2D RotationInputValue;
};
