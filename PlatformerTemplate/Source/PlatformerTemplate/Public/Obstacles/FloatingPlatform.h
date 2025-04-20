// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

class UTimelineComponent;
class UCurveFloat;

UCLASS()
class PLATFORMERTEMPLATE_API AFloatingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingPlatform();

	void PlayFloating();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float TimeOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	UTimelineComponent* FloatingTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	UCurveFloat* FloatingCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector TargetLocation = FVector(0.0f, 0.0f, 200.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WaitTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsMovable = true;
private:
	FVector InitialLocation;

	FVector CurrentTarget;

	bool bMovingToTarget = true;

	bool bIsWaiting = false;

	float CUrrentWaitTimer = 0.0f;

	void ReverseMovement();

	void StartNextMovement();

protected:

	UFUNCTION()
	void UpdateFloatingMovement(float Value);

	UFUNCTION()
	void OnTimelineFinished();
};
