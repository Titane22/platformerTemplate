// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacles/Door.h"
#include "Obstacles/ClearKey.h"
#include "Mario64Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorThreshold = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Threshold"));
	DoorThreshold->SetupAttachment(RootComponent);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh"));
	DoorMesh->SetupAttachment(DoorThreshold);

	RootComponent = DoorThreshold;

	OpenableCheckVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Openable Check Volume"));
	OpenableCheckVolume->SetupAttachment(DoorThreshold);
	OpenableCheckVolume->OnComponentBeginOverlap.AddDynamic(this, &ADoor::OnOverlapBegin);

	OpenTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("OpenTimeline"));
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
	if (OpenCurve)
	{
		FOnTimelineFloat OpenUpdate;
		OpenUpdate.BindUFunction(this, FName("UpdateDoorRotation"));
		OpenTimeline->AddInterpFloat(OpenCurve, OpenUpdate);
	}
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADoor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		if (AMario64Character* Player = Cast<AMario64Character>(OtherActor))
		{
			if (AClearKey* ClearKey = Player->GetClearKey())
			{
				ClearKey->Destroy();
				OpenTheDoor();
			}
		}
	}
}

void ADoor::UpdateDoorRotation(float Value)
{
	FRotator NewRotation = FRotator(0.0f, Value * 90.0f, 0.0f);
	DoorMesh->SetRelativeRotation(NewRotation);
}

void ADoor::OpenTheDoor()
{
	if (OpenSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), OpenSound, GetActorLocation());
	}
	OpenTimeline->PlayFromStart();
}
