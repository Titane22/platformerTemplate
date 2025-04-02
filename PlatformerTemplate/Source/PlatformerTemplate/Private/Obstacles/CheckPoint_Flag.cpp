// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacles/CheckPoint_Flag.h"
#include "../PlatformerTemplateGameMode.h"
#include "Mario64Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACheckPoint_Flag::ACheckPoint_Flag()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FlagMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Flag Mesh"));
	FlagMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FlagMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	FlagMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	FlagMesh->SetupAttachment(RootComponent);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	BoxCollision->SetupAttachment(FlagMesh);
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ACheckPoint_Flag::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ACheckPoint_Flag::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACheckPoint_Flag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACheckPoint_Flag::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMario64Character* Player = Cast<AMario64Character>(OtherActor))
	{
		if (APlatformerTemplateGameMode* GameMode = Cast<APlatformerTemplateGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			GameMode->LastCheckPoint = this;
		}
	}
}


