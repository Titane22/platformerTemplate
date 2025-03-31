// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RollingBridge.generated.h"

UCLASS()
class PLATFORMERTEMPLATE_API ARollingBridge : public AActor
{
    GENERATED_BODY()
    
public:    
    // Sets default values for this actor's properties
    ARollingBridge();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:    
    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    UStaticMeshComponent* StaticMesh;

    // 회전 관련 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rolling Settings")
    float RotationSpeed = 30.0f; 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rolling Settings")
    bool bIsRotating = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rolling Settings")
    TEnumAsByte<EAxis::Type> RotationAxis = EAxis::Z;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rolling Settings")
    bool bUseSineWaveRotation = false; 
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rolling Settings", meta = (EditCondition = "bUseSineWaveRotation"))
    float OscillationAmplitude = 45.0f;  // 진폭 (최대 회전 각도)
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rolling Settings", meta = (EditCondition = "bUseSineWaveRotation"))
    float OscillationFrequency = 0.5f;  // 주파수 (1초당 사이클 수)
    
    // 내부 타이머
    float ElapsedTime = 0.0f;
};