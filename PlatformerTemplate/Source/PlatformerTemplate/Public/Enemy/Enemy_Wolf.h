// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/PT_Enemy.h"
#include "Enemy_Wolf.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMERTEMPLATE_API AEnemy_Wolf : public APT_Enemy
{
	GENERATED_BODY()
	
public:
	AEnemy_Wolf();

	virtual void Die_Implementation();

	virtual void OnDamaged_Implementation();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDamage = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRange = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackCooldown = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	UAnimMontage* DieMont;
};
