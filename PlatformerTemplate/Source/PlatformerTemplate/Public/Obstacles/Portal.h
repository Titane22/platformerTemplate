// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Portal.generated.h"

UCLASS()
class PLATFORMERTEMPLATE_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal();

	void SetLevel(FName ToSetLevelName) { TargetLevelName = ToSetLevelName; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void UpdatePortalEffect(float Value);

	UFUNCTION()
	void ActivatePortal(AActor* OverlappingActor);

	UFUNCTION()
	void TravelToLevel();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* PortalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* PortalEffectMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* PortalTriggerVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UTimelineComponent* PortalEffectTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	UCurveFloat* PortalEffectCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* PortalActivateSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	FName TargetLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	FLinearColor FadeColor = FLinearColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	float FadeDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	float PortalCooldown = 2.0f;

	bool bIsPortalReady = true;

	AActor* CurrentOverlappingActor;

	FTimerHandle PortalCooldownTimerHandle;
	FTimerHandle LevelTravelTimerHandle;
};
