// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Coin.h"
#include "Chest.generated.h"

UCLASS()
class PLATFORMERTEMPLATE_API AChest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void UpdateDoorRotation(float Value);

	void OpenTheDoor();

	void SpawnCoins();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* ChestBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* ChestLid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* ChestCoins;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* OpenableCheckVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UTimelineComponent* OpenTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Curve")
	UCurveFloat* OpenCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* OpenSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	TSubclassOf<ACoin> CoinClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	int32 CoinCount = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	float CoinSpreadForce = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	float CoinUpwardForce = 1000.0f;

	bool bCoinSpawnd = false;

	bool bIsOpened = false;
};
