// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LakituCamera.generated.h"

class UCameraComponent;
class USpringArmComponent;
class AMario64Character;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PLATFORMERTEMPLATE_API ULakituCamera : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULakituCamera();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitializeCamera();

	void SmoothCameraPosition(float DeltaTime);

	void HandleCameraCollision();

	void ResetCamera();

	void RotateCameraAroundTarget(float Yaw, float Pitch);

	void AdjustCameraForMovement(float DeltaTime);

	void CheckCharacterGrounded();

	void UpdateCameraXYPosition();

	bool IsCharacterInCameraView();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	float DefaultDistance = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	float ClkoseDistance = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	float CameraHeight = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	float CameraSmoothSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	float CollisionSmoothSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	float MaxCameraOffset = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	float CameraOffsetSmoothing = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	float MovementThreshold = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	float PitchSensitivity = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	float FloorCheckDist = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings | Spring Arm")
	FVector PreviousCharacterLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings | Spring Arm")
	bool bIsCharacterGrounded;

	FVector LastImpactPoint;
public:	
	AMario64Character* CharacterRef;

private:
	UPROPERTY()
	UCameraComponent* Camera;

	USpringArmComponent* CameraBoom;

	FVector TargetCameraPosition;

	FRotator TargetCameraRotation;

	float TargetArmLength;

	float CurrentInterpolationSpeed;

	bool bWasColliding;

	float CollisionAdjustmentFactor;

	FVector CurrentCameraOffset;

	FVector TargetCameraOffset;
};
