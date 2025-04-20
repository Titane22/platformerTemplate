// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacles/FloatingPlatform.h"
#include "Components/TimelineComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMesh->SetupAttachment(RootComponent);
	StaticMesh->SetSimulatePhysics(false);

	FloatingTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Floating Timeline"));
}

void AFloatingPlatform::PlayFloating()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[this]() {
			FloatingTimeline->PlayFromStart();
		},
		TimeOffset,
		false
	);
}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = GetActorLocation();
	CurrentTarget = InitialLocation + TargetLocation;

	if (FloatingCurve)
	{
		FOnTimelineFloat FloatingEvent;
		FloatingEvent.BindUFunction(this, FName("UpdateFloatingMovement"));
		FloatingTimeline->AddInterpFloat(FloatingCurve, FloatingEvent);

		FOnTimelineEvent FloatingFinished;
		FloatingFinished.BindUFunction(this, FName("OnTimelineFinished"));
		FloatingTimeline->SetTimelineFinishedFunc(FloatingFinished);

		if (bIsMovable)
		{
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle,
				[this]() {
					FloatingTimeline->PlayFromStart();
				},
				TimeOffset,
				false
			);
		}
	}
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsWaiting)
	{
		CUrrentWaitTimer += DeltaTime;

		if (CUrrentWaitTimer >= WaitTime)
		{
			bIsWaiting = false;
			StartNextMovement();
		}
	}
}

void AFloatingPlatform::ReverseMovement()
{
	bMovingToTarget = !bMovingToTarget;
}

void AFloatingPlatform::StartNextMovement()
{
	ReverseMovement();

	FloatingTimeline->PlayFromStart();
}

void AFloatingPlatform::UpdateFloatingMovement(float Value)
{
	FVector NewLocation;

	if (bMovingToTarget)
	{
		NewLocation = FMath::Lerp(InitialLocation, CurrentTarget, Value);
	}
	else
	{
		NewLocation = FMath::Lerp(CurrentTarget, InitialLocation, Value);
	}

	SetActorLocation(NewLocation);
}

void AFloatingPlatform::OnTimelineFinished()
{
	bIsWaiting = true;
	CUrrentWaitTimer = 0.0f;
}

