// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacles/Portal.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Portal Base"));
	PortalMesh->SetCollisionProfileName(TEXT("OverlapAll"));
	RootComponent = PortalMesh;

	PortalTriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Portal Trigger Volume"));
	PortalTriggerVolume->SetupAttachment(PortalMesh);
	PortalTriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
	PortalTriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnOverlapBegin);

	PortalEffectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Portal Effect Mesh"));
	PortalEffectMesh->SetupAttachment(PortalMesh);
	PortalEffectMesh->SetCollisionProfileName(TEXT("NoCollision"));

	PortalEffectTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Portal Effect Timeline"));
}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();
	
	if (PortalEffectCurve)
	{
		FOnTimelineFloat EffectUpdate;
		EffectUpdate.BindUFunction(this, FName("UpdatePortalEffect"));
		PortalEffectTimeline->AddInterpFloat(PortalEffectCurve, EffectUpdate);
	}
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
	if (PlayerCharacter && bIsPortalReady)
	{
		ActivatePortal(PlayerCharacter);
	}
}

void APortal::UpdatePortalEffect(float Value)
{
	if (PortalEffectMesh)
	{
		float Scale = 1.0f + Value * 0.5f; 
		PortalEffectMesh->SetRelativeScale3D(FVector(Scale, Scale, Scale));
		PortalEffectMesh->AddLocalRotation(FRotator(0.0f, Value * 5.0f, 0.0f));
	}
}

void APortal::ActivatePortal(AActor* OverlappingActor)
{
	if (!bIsPortalReady || TargetLevelName.IsNone())
		return;

	bIsPortalReady = false;
	CurrentOverlappingActor = OverlappingActor;
	
	if (PortalEffectTimeline && !PortalEffectTimeline->IsPlaying())
	{
		PortalEffectTimeline->PlayFromStart();
	}
	
	if (PortalActivateSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PortalActivateSound, GetActorLocation());
	}
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		PC->PlayerCameraManager->StartCameraFade(0.0f, 1.0f, FadeDuration / 2.0f, FadeColor);
	}
	
	GetWorldTimerManager().SetTimer(
		LevelTravelTimerHandle,
		this,
		&APortal::TravelToLevel,
		FadeDuration / 2.0f,
		false
	);
}

void APortal::TravelToLevel()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);
	CurrentOverlappingActor = nullptr;
	UGameplayStatics::OpenLevel(GetWorld(), TargetLevelName);
}

