// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Door.generated.h"

class APortal;
class ACheckPoint_Flag;

UCLASS()
class PLATFORMERTEMPLATE_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void UpdateDoorRotation(float Value);

	UFUNCTION()
	void FinishedDoorRotation();

	UFUNCTION(BlueprintCallable, Category = "Action")
	void OpenTheDoor();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* DoorThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* OpenableCheckVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UTimelineComponent* OpenTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* PortalPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Curve")
	UCurveFloat* OpenCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* OpenSound;

	bool bIsOpenable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
	TSubclassOf<APortal> BP_PortalClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
	APortal* Portal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
	FName TargetLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
	ACheckPoint_Flag* ToMoveFlag;
};
