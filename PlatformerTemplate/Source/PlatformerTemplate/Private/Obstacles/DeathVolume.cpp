// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacles/DeathVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Mario64Character.h"
#include "Engine/DamageEvents.h"

// Sets default values
ADeathVolume::ADeathVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create and setup the trigger volume
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	RootComponent = TriggerVolume;
	TriggerVolume->SetBoxExtent(FVector(100.0f, 100.0f, 50.0f));
}

// Called when the game starts or when spawned
void ADeathVolume::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind overlap event
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ADeathVolume::OnOverlapBegin);
}

// Called every frame
void ADeathVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADeathVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMario64Character* Player = Cast<AMario64Character>(OtherActor))
	{
		FPointDamageEvent DamageEvent(1000.0f, SweepResult, -FVector::UpVector, nullptr);
		Player->TakeDamage(1000.0f, DamageEvent, nullptr, this);
	}
}

