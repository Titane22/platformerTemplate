// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacles/RollingBridge.h"

// Sets default values
ARollingBridge::ARollingBridge()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 5.0f));
	StaticMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	StaticMesh->SetSimulatePhysics(false);
	RootComponent = StaticMesh;
	
}

// Called when the game starts or when spawned
void ARollingBridge::BeginPlay()
{
	Super::BeginPlay();
	ElapsedTime = 0.0f;
}

// Called every frame
void ARollingBridge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bIsRotating)
		return;
		
	// 타이머 업데이트
	ElapsedTime += DeltaTime;
	
	// 현재 회전값 가져오기
	FRotator CurrentRotation = GetActorRotation();
	FRotator NewRotation = CurrentRotation;
	
	if (bUseSineWaveRotation)
	{
		// 삼각함수를 사용한 주기적 회전
		// sin() 함수는 -1에서 1 사이의 값을 반환하므로 진폭을 곱해줍니다
		float SineValue = FMath::Sin(ElapsedTime * OscillationFrequency * 2.0f * PI);
		float TargetAngle = SineValue * OscillationAmplitude;
		
		NewRotation.Pitch = TargetAngle;
	}
	else
	{
		// 기존의 일정 속도 회전
		switch (RotationAxis)
		{
		case EAxis::X:
			{
				// X축(Roll) 회전 - 로컬 전방 벡터(X축)를 기준으로 회전
				FQuat LocalRotation = FQuat(GetActorForwardVector(), FMath::DegreesToRadians(RotationSpeed * DeltaTime));
				FQuat CurrentQuat = GetActorQuat();
				FQuat NewQuat = LocalRotation * CurrentQuat;
				NewRotation = NewQuat.Rotator();
			}
			break;
		case EAxis::Y:
			{
				// Y축(Pitch) 회전 - 로컬 오른쪽 벡터(Y축)를 기준으로 회전
				FQuat LocalRotation = FQuat(GetActorRightVector(), FMath::DegreesToRadians(RotationSpeed * DeltaTime));
				FQuat CurrentQuat = GetActorQuat();
				FQuat NewQuat = LocalRotation * CurrentQuat;
				NewRotation = NewQuat.Rotator();
			}
			break;
		case EAxis::Z:
		default:
			{
				// Z축(Yaw) 회전 - 로컬 위쪽 벡터(Z축)를 기준으로 회전
				FQuat LocalRotation = FQuat(GetActorUpVector(), FMath::DegreesToRadians(RotationSpeed * DeltaTime));
				FQuat CurrentQuat = GetActorQuat();
				FQuat NewQuat = LocalRotation * CurrentQuat;
				NewRotation = NewQuat.Rotator();
			}
			break;
		}
	}
	
	// 새 회전값 적용
	SetActorRotation(NewRotation);
}

