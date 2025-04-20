// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacles/Chest.h"
#include "Kismet/GameplayStatics.h"
#include "Mario64Character.h"

// Sets default values
AChest::AChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ChestBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chest Base"));
	ChestBase->SetupAttachment(RootComponent);

	ChestLid = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chest Lid"));
	ChestLid->SetupAttachment(ChestBase);

	ChestCoins = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chest Coins"));
	ChestCoins->SetupAttachment(ChestBase);

	OpenableCheckVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Openable Check Volume"));
	OpenableCheckVolume->SetupAttachment(ChestBase);
	OpenableCheckVolume->OnComponentBeginOverlap.AddDynamic(this, &AChest::OnOverlapBegin);

	RootComponent = ChestBase;

	OpenTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("OpenTimeline"));
}

// Called when the game starts or when spawned
void AChest::BeginPlay()
{
	Super::BeginPlay();
	
	if (OpenCurve)
	{
		FOnTimelineFloat OpenUpdate;
		OpenUpdate.BindUFunction(this, FName("UpdateDoorRotation"));
		OpenTimeline->AddInterpFloat(OpenCurve, OpenUpdate);
	}
}

void AChest::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		if (AMario64Character* Player = Cast<AMario64Character>(OtherActor))
		{
			OpenTheDoor();
		}
	}
}

void AChest::UpdateDoorRotation(float Value)
{
	FRotator NewRotation = FRotator(0.0f, 0.0f, Value * -45.0f);
	ChestLid->SetRelativeRotation(NewRotation);

	if (Value >= 1.0f && !bCoinSpawnd && !bIsOpened)
	{
		SpawnCoins();
		bCoinSpawnd = true;
	}
}

void AChest::OpenTheDoor()
{
	if (OpenSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), OpenSound, GetActorLocation());
	}
	OpenTimeline->PlayFromStart();
}

void AChest::SpawnCoins()
{
	if (!CoinClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("AChest::SpawnCoins() CoinClass is Null"));
		return;
	}
	FVector SpawnLocation = ChestLid->GetComponentLocation() + FVector(0.0f, 0.0f, 50.0f);
	FVector ForwardVector = GetActorForwardVector();
	FVector RightVector = FVector::CrossProduct(ForwardVector, FVector::UpVector);

	for (int32 idx = 0; idx < CoinCount; idx++)
	{
		ACoin* Coin = GetWorld()->SpawnActor<ACoin>(
			CoinClass,
			SpawnLocation,
			FRotator::ZeroRotator
		);

		if (Coin)
		{
			Coin->StopTimeline();
			float RandomAngle = FMath::RandRange(-90.0f, 90.0f);
			float RandomForce = FMath::RandRange(0.5f, 1.0f) * CoinSpreadForce;

			/*FVector RandomDirection = FVector(
				FMath::Cos(FMath::DegreesToRadians(RandomAngle)),
				FMath::Sin(FMath::DegreesToRadians(RandomAngle)),
				0.0f
			);*/

			FVector RandomDirection = ForwardVector.RotateAngleAxis(RandomAngle, FVector::UpVector);

			if (UStaticMeshComponent* CoinMesh = Coin->FindComponentByClass<UStaticMeshComponent>())
			{
				CoinMesh->SetSimulatePhysics(true);
				CoinMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

				FVector ImpulseForce = RandomDirection * RandomForce + FVector(0, 0, CoinUpwardForce);
				CoinMesh->AddImpulse(ImpulseForce);

				FTimerHandle TimerHandle;
				GetWorldTimerManager().SetTimer(
					TimerHandle,
					[Coin, CoinMesh]() {
						if (CoinMesh)
						{
							CoinMesh->SetSimulatePhysics(false);
						}
						Coin->PlayTimeline();
					},
					1.1f,
					false
				);
			}
		}
		ChestCoins->SetVisibility(false);
	}
}

