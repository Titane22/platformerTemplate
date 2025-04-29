// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/InterpToMovementComponent.h"
#include "MovablePillar.generated.h"

UCLASS()
class PLATFORMERTEMPLATE_API AMovablePillar : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovablePillar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* PillarMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	USplineComponent* MovementSpline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UInterpToMovementComponent* PillarMovement;

	float RotationPerTick = 5.0f;
};
