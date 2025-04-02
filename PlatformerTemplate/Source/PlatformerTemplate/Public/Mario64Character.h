// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../PlatformerTemplateCharacter.h"
#include "Mario64Character.generated.h"

class ULakituCamera;
class ALadder;

UENUM(BlueprintType)
enum class EJumpState : uint8
{
	Single	UMETA(DisplayName = "Single"),
	Double	UMETA(DisplayName = "Double"),
	Triple	UMETA(DisplayName = "Triple")
};

UCLASS()
class PLATFORMERTEMPLATE_API AMario64Character : public APlatformerTemplateCharacter
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

public:
	AMario64Character();

	/** 카메라 시스템을 강제로 리셋 */
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void ResetCameraSystem() { bNeedCameraReset = true; }

	void ToggleClimbing(bool ToSetState, ALadder* ToSetLadde);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaSeconds);
	
	virtual void Move(const FInputActionValue& Value);

	virtual void Look(const FInputActionValue& Value);

	virtual void Jump() override;

	void PerformLongJump();

	virtual void StopJumping() override;

	virtual void Landed(const FHitResult& Hit);

	void Sprint(const FInputActionValue& Value);

	void UpdateMovementDirection(FVector NewDirection);

	void CheckForTurn();

	void Crouch(const FInputActionValue& Value);

	bool IsWall(UPrimitiveComponent* Component, const FHitResult& Hit);

	void PerformWallJump();

	void ResetLevel();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse,
		const FHitResult& Hit);

	void OnWallHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse,
		const FHitResult& Hit);

	void OnObjectHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse,
		const FHitResult& Hit);

	UFUNCTION()
	void OnSideSomersaultMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintNativeEvent, Category = "Health")
	void Die();
	virtual void Die_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Health")
	void OnDamaged();
	virtual void OnDamaged_Implementation();

	UFUNCTION()
	void EndInvulnerability();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	ULakituCamera* LakituCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* LegPoint;

	/** 카메라 리셋이 필요한지 여부 */
	bool bNeedCameraReset = false;

	/** 점프 상태 (단일/이중/삼중) */
	EJumpState JumpState = EJumpState::Single;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	UAnimMontage* UTurnMont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	float NextJumpMaxIdleTime = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	float UTurnIdleTime = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	float WallJumpIdleTime = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	float LongJumpIdelTime = 0.1f;

	float CurrentJumpTime = 0.0f;

	float CurrentMoveTime = 0.0f;

	float CurrentWallHitTime = 0.0f;

	float CurrentCrouchTime = 0.0f;

	FVector CurrentDirection = FVector::ZeroVector;

	FVector PreviousDirection;

	bool bIsSprinting = false;

	bool bIsCrouching = false;
	
	bool bIsUTurn = false;

	bool bIsJumping = false;

	bool bCanWallJump = false;

	bool bWasRunning = false;

	bool bCameDownLadder = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climbing")
	bool bIsClimbing = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climbing")
	bool bClimbingHasInput = false;

	FVector LastWallNormal;

	UPROPERTY()
	class ALadder* CurrentLadderRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentHealth = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float InvulnerabilityTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool bIsInvulnerable = false;

	FTimerHandle InvulnerabilityTimerHandle;
};
