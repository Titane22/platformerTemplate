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
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Throw();
	void HoldActor();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* PickingPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ThrowAction;

	bool bIsTakingPotato = false;

	bool bIsHoldingActor = false;

	UPROPERTY()
	AActor* HeldActor;
};
