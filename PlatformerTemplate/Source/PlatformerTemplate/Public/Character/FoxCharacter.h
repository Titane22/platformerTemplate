// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/PlayerInterface.h"
#include "Mario64Character.h"
#include "FoxCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMERTEMPLATE_API AFoxCharacter : public AMario64Character
{
	GENERATED_BODY()

public:
	AFoxCharacter();

	virtual void BeginPlay() override;
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Throw();
	
	void HoldActor();

	void ThrowHeldActor();

	UFUNCTION()
	void OnThrownCharacterHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
							UPrimitiveComponent* OtherComp, FVector NormalImpulse, 
							const FHitResult& Hit);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* PickingPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ThrowAction;

	bool bIsTakingPotato = false;

	bool bIsHoldingActor = false;

	UPROPERTY()
	AActor* HeldActor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwing")
	float ThrowForce = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwing")
	float ThrowUpwardForce = 300.0f;

	float OriginSprintSpeed;
	float OriginWalkSpeed;

	float HoldingSprintSpeed = 150.0f;
	float HoldingWalkSpeed = 75.0f;
};
