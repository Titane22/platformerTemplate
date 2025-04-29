// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/PT_Enemy.h"
#include "Enemy_Bull.generated.h"

class UBoxComponent;
class AMario64Character;

UENUM()
enum class EBullState : uint8
{
	Idle,
	Attack,
	Stun,
	Dead
};

/**
 * 
 */
UCLASS()
class PLATFORMERTEMPLATE_API AEnemy_Bull : public APT_Enemy
{
	GENERATED_BODY()
	
public:
	AEnemy_Bull();

	AMario64Character* GetTargetPlayer();
	
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	AMario64Character* FindPlayer(TSubclassOf<AMario64Character> ToFindCharacter);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void HandleIdle();

	void HandleStun();

	void HandleAttack();

	virtual void Die_Implementation();
	virtual void OnDamaged_Implementation();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* Box;

	AMario64Character* Potato;

	AMario64Character* Fox;

	AMario64Character* TargetPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	USoundBase* CowSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	USoundBase* RushSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	USoundBase* StunSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "States")
	EBullState CurrentState = EBullState::Idle;
	
	FVector AttackDirection;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float PawingTime = 2.0f;

	float StunTimer = 0.0f;

};
