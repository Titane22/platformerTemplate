// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HavingKeySquirrel.generated.h"

class AClearKey;

UCLASS()
class PLATFORMERTEMPLATE_API AHavingKeySquirrel : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHavingKeySquirrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void SpawnRealKey();

	bool IsGround(const FHitResult& Hit);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void ThrowAcorn(AActor* HitActor);

public:	
	UFUNCTION(BlueprintCallable, Category = "Extract")
	void ExtractKey();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* SquirrelMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* KeyMeshComponent;

	AClearKey* HavingKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Extract")
	TSubclassOf<AClearKey> ClearKeyClass;

	float TimeSinceThrow = 0.0f;

	bool bKeyThrown = false;

	bool bKeyHitGround = false;

	UPROPERTY(EditDefaultsOnly, Category = "Extract")
	float ThrowStrength = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Extract")
	float UpwardStrength = 300.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AActor> AcornClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	USoundBase* AcornThrowingSound;

	float AcornThrowingCoolTime = 5.0f;

	float CurrentThrowingTime = 0.0f;
};
