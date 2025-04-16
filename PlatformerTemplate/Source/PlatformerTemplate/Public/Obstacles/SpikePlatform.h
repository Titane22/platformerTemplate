// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpikePlatform.generated.h"

class UTimelineComponent;
class UCurveFloat;

UCLASS()
class PLATFORMERTEMPLATE_API ASpikePlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpikePlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void UpdateSpikeElevating(float Value);

	UFUNCTION()
	void OnTimelineFinished();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* FloorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* SpikeMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UTimelineComponent* SpikeTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike")
	UCurveFloat* SpikeCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* RisingSpikeSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DescendSpeedMultiplier = 0.5f;

	float SpikeUp = 30.0f;

	float SpikeDown = -30.0f;

	bool bIsRising = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float SoundStartTime = 0.15f;
};
