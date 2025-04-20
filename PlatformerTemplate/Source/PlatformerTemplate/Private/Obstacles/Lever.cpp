// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacles/Lever.h"
#include "Obstacles/FloatingPlatform.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALever::ALever()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LeverBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lever Base"));
	LeverBase->SetupAttachment(RootComponent);

	ActivatableCheckVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Activatable Check Volume"));
	ActivatableCheckVolume->SetupAttachment(LeverBase);
	ActivatableCheckVolume->OnComponentBeginOverlap.AddDynamic(this, &ALever::OnOverlapBegin);

	LeverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lever Mesh"));
	LeverMesh->SetupAttachment(LeverBase);

	ActivateTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ActivateTimeline"));
}

// Called when the game starts or when spawned
void ALever::BeginPlay()
{
	Super::BeginPlay();
	
	OriginRotation = LeverMesh->GetRelativeRotation();
	if (ActivateCurve)
	{
		FOnTimelineFloat OpenUpdate;
		OpenUpdate.BindUFunction(this, FName("UpdateFloorRotation"));
		ActivateTimeline->AddInterpFloat(ActivateCurve, OpenUpdate);
	}

}

// Called every frame
void ALever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALever::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && !bIsActivaable)
	{
		ActivateLever();
		ActivatableCheckVolume->OnComponentBeginOverlap.RemoveDynamic(this, &ALever::OnOverlapBegin);
		if (TargetRef)
		{
			if (AFloatingPlatform* FloatingPlatform = Cast<AFloatingPlatform>(TargetRef))
			{
				FloatingPlatform->PlayFloating();
			}
		}
	}
}

void ALever::UpdateFloorRotation(float Value)
{
	FRotator NewRotation = FRotator(OriginRotation.Pitch - Value * 90.0f, 0.0f, 0.0f);
	LeverMesh->SetRelativeRotation(NewRotation);
}

void ALever::ActivateLever()
{
	if (ActivateSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ActivateSound, GetActorLocation());
	}
	bIsActivaable = true;
	ActivateTimeline->PlayFromStart();
}
