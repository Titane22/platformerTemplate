// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacles/Ladder.h"
#include "Components/BoxComponent.h"
#include "Mario64Character.h"

// Sets default values
ALadder::ALadder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LadderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ladder"));
	LadderMesh->SetupAttachment(RootComponent);

	PlayerCheckCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Player Checker"));
	PlayerCheckCollider->SetupAttachment(LadderMesh);
	PlayerCheckCollider->SetGenerateOverlapEvents(true);
	/*PlayerCheckCollider->OnComponentBeginOverlap.AddDynamic(this, &ALadder::OnPlayerClimbCheckBeginOverlap);
	PlayerCheckCollider->OnComponentEndOverlap.AddDynamic(this, &ALadder::OnPlayerClimbCheckEndOverlap);*/
}

// Called when the game starts or when spawned
void ALadder::BeginPlay()
{
	Super::BeginPlay();

	PlayerCheckCollider->OnComponentBeginOverlap.AddDynamic(this, &ALadder::OnPlayerClimbCheckBeginOverlap);
	PlayerCheckCollider->OnComponentEndOverlap.AddDynamic(this, &ALadder::OnPlayerClimbCheckEndOverlap);
}


// Called every frame
void ALadder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALadder::OnPlayerClimbCheckBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMario64Character* Player = Cast<AMario64Character>(OtherActor))
	{
		Player->ToggleClimbing(true, this);
	}
}

void ALadder::OnPlayerClimbCheckEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AMario64Character* Player = Cast<AMario64Character>(OtherActor))
	{
		Player->ToggleClimbing(false, nullptr);
	}
}
