// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacles/MovablePillar.h"
#include "Components/InterpToMovementComponent.h"
#include "Mario64Character.h"

// Sets default values
AMovablePillar::AMovablePillar()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	PillarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pillar Mesh"));
	PillarMesh->SetupAttachment(RootComponent);

	MovementSpline = CreateDefaultSubobject<USplineComponent>(TEXT("MovementSpline"));
	MovementSpline->SetupAttachment(RootComponent);

	PillarMovement = CreateDefaultSubobject<UInterpToMovementComponent>(TEXT("PillarMovement"));
	PillarMovement->UpdatedComponent = RootComponent;
	PillarMovement->BehaviourType = EInterpToBehaviourType::PingPong;
	PillarMovement->bAutoActivate = true; 
	PillarMovement->Duration = 2.0f; 
}

// Called when the game starts or when spawned
void AMovablePillar::BeginPlay()
{
	Super::BeginPlay();
	PillarMesh->OnComponentHit.AddDynamic(this, &AMovablePillar::OnHit);

	if (PillarMovement && PillarMovement->ControlPoints.Num() > 1)
	{
		// 컨트롤 포인트가 있을 경우 활성화
		if (!PillarMovement->IsActive())
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Activate"));
			PillarMovement->Activate();
		}
	}
}

void AMovablePillar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	PillarMesh->AddRelativeRotation(FRotator(0.0f, RotationPerTick, 0.0f));
	//PillarMesh->SetWorldRotation(FRotator(0.0f, 0.0f, 0.0f));
}

void AMovablePillar::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this)
	{
		if (AMario64Character* Player = Cast<AMario64Character>(OtherActor))
		{
			FVector Direction = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			Player->LaunchCharacter(Direction * 2000, false, false);
		}
	}
}
