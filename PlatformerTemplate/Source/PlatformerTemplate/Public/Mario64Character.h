// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../PlatformerTemplateCharacter.h"
#include "AI/PlayerInterface.h"
#include "Mario64Character.generated.h"

class ULakituCamera;
class ALadder;
class AClearKey;

UENUM(BlueprintType)
enum class EJumpState : uint8
{
	Single	UMETA(DisplayName = "Single"),
	Double	UMETA(DisplayName = "Double"),
	Triple	UMETA(DisplayName = "Triple")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	Idle		UMETA(DisplayName = "Idle"),
	Flying		UMETA(DisplayName = "Flying"),
	Crouching	UMETA(DisplayName = "Crouching"),
	Climbing	UMETA(DisplayName = "Climbing"),
	Sliding		UMETA(DisplayName = "Sliding"),
	Die			UMETA(DisplayName = "Die"),

	/// For Potato Character
	Burrowed	UMETA(DisplayName = "Burrowed"),

	/// For Fox Character
	Helding		UMETA(DisplayName = "Helding")
};

UCLASS(config = Game, BlueprintType, hideCategories = (Navigation))
class PLATFORMERTEMPLATE_API AMario64Character : public APlatformerTemplateCharacter, public IPlayerInterface
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

public:
	AMario64Character();

	UFUNCTION(BlueprintCallable, Category = "Interface")
	virtual void JumpToDestination(FVector Destination);

	/** 카메라 시스템을 강제로 리셋 */
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void ResetCameraSystem() { bNeedCameraReset = true; }

	void ToggleClimbing(bool ToSetState, ALadder* ToSetLadde);

	void SetClearKey(AClearKey* ToSetKey);

	AClearKey* GetClearKey() const { return ClearKeyRef.Get(); }

	void ClearKeyReference() { ClearKeyRef = nullptr; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Jump() override;

	void SetPartner(AMario64Character* ToSetPartner);

	void MovementSwitchOnOff();
	
	bool IsStopFromPartner() { return bIsStopFromPartner; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	const float GetMaxHealth() { return MaxHealth; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	const float GetCurrentHealth() { return CurrentHealth; }

	EActionState GetPlayerState() { return CurrentState; }

	AMario64Character* GetPartner() const { return PartnerRef; }

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaSeconds);
	
	virtual void Move(const FInputActionValue& Value);

	virtual void Look(const FInputActionValue& Value);

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

	void SetState(const EActionState State);

	void TagCharacter();

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

	void CallStop();

protected:
	EActionState CurrentState = EActionState::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* TagAction;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	float CurrentJumpTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* SingleJumpSound;

	float CurrentMoveTime = 0.0f;

	float CurrentWallHitTime = 0.0f;

	float CurrentCrouchTime = 0.0f;

	FVector CurrentDirection = FVector::ZeroVector;

	FVector PreviousDirection;

	bool bIsSprinting = false;

	bool bIsUTurn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	bool bUseU_Turn = true;

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
	float MaxHealth = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentHealth = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float InvulnerabilityTime = 2.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	bool bIsInvulnerable = false;

	bool bHasKey = false;

	UPROPERTY()
	TWeakObjectPtr<AClearKey> ClearKeyRef = nullptr;

	FTimerHandle InvulnerabilityTimerHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	float IdleGravityScale = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	float DoubleJumpGravityScale = 3.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	float TripleJumpGravityScale = 3.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	float DisableGravityScale = 0.0f;

	bool bIsStopFromPartner = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintMaxSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkMaxSpeed = 230.0f;

	AMario64Character* PartnerRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* StopCommandAction;
};
