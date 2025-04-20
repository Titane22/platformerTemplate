// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mario64Character.h"
#include "Components/TimelineComponent.h"
#include "Components/BoxComponent.h"
#include "InputActionValue.h"
#include "PotatoCharacter.generated.h"

#define COL_MAX 32
/**
 * 
 */
UCLASS(config=Game, BlueprintType, hideCategories=(Navigation))
class PLATFORMERTEMPLATE_API APotatoCharacter : public AMario64Character
{
	GENERATED_BODY()

public:
	APotatoCharacter();

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// Check if Plat Ground
	bool CheckPlatGround();

	// Call Burrow for Input Action
	void ToogleBurrow(const FInputActionValue& Value);

	void Burrow();

	void Unburrow();
	
	void SaveOriginalCollisionResponses();

	void ImpulseForceOnPotatoHead();

	UFUNCTION()
	virtual void OnBurrowBoxHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse,
		const FHitResult& Hit);

	//UFUNCTION()
	//void UpdateBurrowHeight(float Value);

	//UFUNCTION()
	//void UpdateUnburrowHeight(float Value);

protected:
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UTimelineComponent* BurrowTimeline;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UTimelineComponent* UnburrowTimeline;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* BurrowBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Curves")
	UCurveFloat* BurrowCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Curves")
	UCurveFloat* UnburrowCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BurrowAction;

	ECollisionEnabled::Type OrgCollisionEnabled;
	ECollisionResponse OriginalCollisionResponses[COL_MAX];

	TArray<AActor*> OnPotatoHead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	FVector LaunchVelocity = FVector(0.0f, 0.0f, 2500.0f);

	float CurrentBurrowTime = 0.0f;

	float BurrowMaxTime = 2.0f;

	float BurrowedHeight = 170.0f;

	float BurrowSpeed = 5.0f;

	bool bWasBurrowed = false;

	bool bIsBurrowed = false;


};
