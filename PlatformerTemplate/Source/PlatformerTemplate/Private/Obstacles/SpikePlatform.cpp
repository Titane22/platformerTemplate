// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacles/SpikePlatform.h"
#include "Components/TimelineComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ASpikePlatform::ASpikePlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor"));
	FloorMesh->SetupAttachment(RootComponent);
	SpikeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spike"));
	SpikeMesh->SetupAttachment(FloorMesh);

	SpikeTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Spike Timeline"));
}

// Called when the game starts or when spawned
void ASpikePlatform::BeginPlay()
{
	Super::BeginPlay();
	
	if (SpikeCurve)
	{
		FOnTimelineFloat TimelineFloat;
		TimelineFloat.BindUFunction(this, FName("UpdateSpikeElevating"));
		SpikeTimeline->AddInterpFloat(SpikeCurve, TimelineFloat);

		FOnTimelineEvent OnFinished;
		OnFinished.BindUFunction(this, FName("OnTimelineFinished"));
		SpikeTimeline->SetTimelineFinishedFunc(OnFinished);
		SpikeTimeline->SetPlayRate(2.0f);

		SpikeTimeline->PlayFromStart();
	}
}

// Called every frame
void ASpikePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpikePlatform::UpdateSpikeElevating(float Value)
{
	float TargetZ;
	if (bIsRising)
	{
		TargetZ = FMath::Lerp(SpikeDown, SpikeUp, Value);
	}
	else
	{
		TargetZ = FMath::Lerp(SpikeUp, SpikeDown, Value);
	}
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("TargetZ: %lf"), TargetZ));
	FVector NewLocation = FVector(0.0f, 0.0f, TargetZ);
	SpikeMesh->SetRelativeLocation(NewLocation);
}

void ASpikePlatform::OnTimelineFinished()
{
	if (bIsRising)
	{
		bIsRising = false;

		SpikeTimeline->SetPlayRate(DescendSpeedMultiplier);
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			[this]() {
				SpikeTimeline->PlayFromStart();
				//SpikeTimeline->Reverse();
			},
			2.0f,
			false
		);
	}
	else
	{
		bIsRising = true;

		SpikeTimeline->SetPlayRate(2.0f);

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			[this]() {
				SpikeTimeline->PlayFromStart();
			},
			2.0f,
			false
		);
	}
}

