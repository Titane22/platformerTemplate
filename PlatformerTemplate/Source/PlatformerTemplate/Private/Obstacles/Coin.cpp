// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacles/Coin.h"
#include "Components/TimelineComponent.h"
#include "Mario64Character.h"
#include "PlatformerGameStateBase.h"
#include "Kismet/GameplayStatics.h"

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
	OriginZ = GetActorLocation().Z;

	if (RotatingDownCurve)
	{
		FOnTimelineFloat DownFloat;
		DownFloat.BindUFunction(this, FName("UpdateRotatingDown"));
		RotatingDownTimeline->AddInterpFloat(RotatingDownCurve, DownFloat);
		RotatingDownTimeline->SetLooping(true);
		RotatingDownTimeline->PlayFromStart();
	}
	if (RotatingUpCurve)
	{
		FOnTimelineFloat UpFloat;
		UpFloat.BindUFunction(this, FName("UpdateRotatingUp"));
		RotatingUpTimeline->AddInterpFloat(RotatingUpCurve, UpFloat);
		RotatingUpTimeline->SetLooping(true);
		RotatingUpTimeline->PlayFromStart();
	}
}

// Called every frame
void ACoin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACoin::PlayTimeline()
{
	if (RotatingDownCurve)
	{
		RotatingDownTimeline->PlayFromStart();
	}
	if (RotatingUpTimeline)
	{
		RotatingUpTimeline->PlayFromStart();
	}
}

void ACoin::StopTimeline()
{
	if (RotatingDownCurve)
	{
		RotatingDownTimeline->Stop();
	}
	if (RotatingUpTimeline)
	{
		RotatingUpTimeline->Stop();
	}
}

void ACoin::UpdateRotatingDown(float Value)
{
	FVector CurrentLocation = CoinMesh->GetRelativeLocation();
	FVector NewLocation = FVector(CurrentLocation.X, CurrentLocation.Y, OriginZ + Value);
	CoinMesh->SetRelativeLocation(NewLocation);
}

void ACoin::UpdateRotatingUp(float Value)
{
	FVector CurrentLocation = CoinMesh->GetRelativeLocation();
	FVector NewLocation = FVector(CurrentLocation.X, CurrentLocation.Y, OriginZ + Value);
	CoinMesh->SetRelativeLocation(NewLocation);
}

void ACoin::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMario64Character* Player = Cast<AMario64Character>(OtherActor))
	{
		if (APlatformerGameStateBase* GameState = Cast<APlatformerGameStateBase>(GetWorld()->GetGameState()))
		{
			GameState->AddScore(CoinValue);
		}
		
		if (GettingCoinSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, GettingCoinSound, GetActorLocation());
		}
		
		Destroy();
	}
}


