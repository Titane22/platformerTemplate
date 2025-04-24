// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BehaviorTree.h"
#include "PT_Enemy.generated.h"

/**
 * 동물형 몬스터 기본 클래스
 */
UCLASS()
class PLATFORMERTEMPLATE_API APT_Enemy : public ACharacter
{
	GENERATED_BODY()
	
public:
	APT_Enemy();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintNativeEvent, Category = "Health")
	void Die();
	virtual void Die_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Health")
	void OnDamaged();
	virtual void OnDamaged_Implementation();
	
	UFUNCTION()
	virtual void OnDieMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
	UFUNCTION()
	void ForceDestroy();

	float GetCurrenntHealth() const { return CurrentHealth; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovementSpeed = 300.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* DieMontage;
	
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	bool bIsDying = false;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float CurrentHealth;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float MaxHealth = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;
};
