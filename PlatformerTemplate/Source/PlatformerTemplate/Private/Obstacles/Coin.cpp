// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacles/Coin.h"
#include "Components/TimelineComponent.h"
#include "Mario64Character.h"

// Sets default values
ACoin::ACoin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Coin Mesh"));
	CoinMesh->SetupAttachment(RootComponent);
	CoinMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CoinMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	CoinMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	RotatingDownTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Rotating Down Timeline"));
	RotatingUpTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Rotating Up Timeline"));
}

// Called when the game starts or when spawned
void ACoin::BeginPlay()
{
	Super::BeginPlay();
	
	CoinMesh->OnComponentBeginOverlap.AddDynamic(this, &ACoin::OnComponentBeginOverlap);

	if (RotatingDownCurve)
	{
		FOnTimelineFloat DownFloat;
		DownFloat.BindUFunction(this, FName("UpdateRotatingDown"));
		RotatingDownTimeline->AddInterpFloat(RotatingDownCurve, DownFloat);

		RotatingDownTimeline->PlayFromStart();
	}
	if (RotatingUpCurve)
	{
		FOnTimelineFloat UpFloat;
		UpFloat.BindUFunction(this, FName("UpdateRotatingUp"));
		RotatingUpTimeline->AddInterpFloat(RotatingUpCurve, UpFloat);
		RotatingUpTimeline->PlayFromStart();
	}
}

// Called every frame
void ACoin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACoin::UpdateRotatingDown(float Value)
{
	FVector CurrentLocation = CoinMesh->GetRelativeLocation();
	FVector NewLocation = FVector(CurrentLocation.X, CurrentLocation.Y, Value);
	CoinMesh->SetRelativeLocation(NewLocation);
}

void ACoin::UpdateRotatingUp(float Value)
{
	FVector CurrentLocation = CoinMesh->GetRelativeLocation();
	FVector NewLocation = FVector(CurrentLocation.X, CurrentLocation.Y, Value);
	CoinMesh->SetRelativeLocation(NewLocation);
}

void ACoin::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMario64Character* Player = Cast<AMario64Character>(OtherActor))
	{
		// TODO: Add coin pickup sound
		
		Destroy();
	}
}


