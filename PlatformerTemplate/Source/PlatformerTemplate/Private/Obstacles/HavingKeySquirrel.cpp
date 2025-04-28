// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacles/HavingKeySquirrel.h"
#include "Obstacles/ClearKey.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AHavingKeySquirrel::AHavingKeySquirrel()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SquirrelMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Squirrel Mesh"));
	SquirrelMesh->SetupAttachment(RootComponent);

	KeyMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Key Mesh"));
	KeyMeshComponent->SetupAttachment(SquirrelMesh, "hand_key_socket");
	KeyMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	KeyMeshComponent->SetSimulatePhysics(false);
}

// Called when the game starts or when spawned
void AHavingKeySquirrel::BeginPlay()
{
	Super::BeginPlay();


}

void AHavingKeySquirrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bKeyThrown || !AcornClass)
		return;

	CurrentThrowingTime += DeltaTime;

	if (CurrentThrowingTime < AcornThrowingCoolTime)
		return;

	FVector StartLocation = SquirrelMesh->GetSocketLocation("hand_key_socket");
	FVector EndLocation = StartLocation + GetActorRightVector() * 500.0f;

	/*DrawDebugLine(
		GetWorld(),
		StartLocation,
		EndLocation,
		FColor::Red
	);*/

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_Pawn,
		QueryParams
	))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Hit!!!!!!!"));
		if (AActor* HitActor = HitResult.GetActor())
		{
			CurrentThrowingTime = 0.0f;
			ThrowAcorn(HitActor);
		}
	}
}

void AHavingKeySquirrel::SpawnRealKey()
{
	if (ClearKeyClass)
	{
		FVector SpawnLocation = KeyMeshComponent->GetComponentLocation() + FVector(0.0f, 0.0f, 50.0f);
		FRotator SpawnRotation = KeyMeshComponent->GetComponentRotation();

		KeyMeshComponent->SetVisibility(false);
		KeyMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		HavingKey = GetWorld()->SpawnActor<AClearKey>(
			ClearKeyClass,
			SpawnLocation,
			SpawnRotation,
			SpawnParams
		);
	}
}

bool AHavingKeySquirrel::IsGround(const FHitResult& Hit)
{
	return Hit.ImpactNormal.Z > 0.7f;
}

void AHavingKeySquirrel::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!bKeyHitGround && IsGround(Hit))
	{
		bKeyHitGround = true;
		SpawnRealKey();
	}
}

void AHavingKeySquirrel::ThrowAcorn(AActor* HitActor)
{
	FVector SpawnLocation = SquirrelMesh->GetSocketLocation("hand_key_socket");

	FVector TargetDirection = HitActor->GetActorLocation() - SpawnLocation;
	TargetDirection.Normalize();

	FRotator SpawnRotation = TargetDirection.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (AActor* SpawnedAcorn = GetWorld()->SpawnActor<AActor>(
		AcornClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	))
	{
		if (AcornThrowingSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				AcornThrowingSound,
				GetActorLocation()
			);
		}
		UPrimitiveComponent* PrimComp = SpawnedAcorn->FindComponentByClass<UPrimitiveComponent>();
		if (PrimComp && PrimComp->IsSimulatingPhysics())
		{
			FVector LaunchDirection = SpawnRotation.Vector();
			PrimComp->AddImpulse(LaunchDirection * 1500.0f);
		}
		else
		{
			if (UProjectileMovementComponent* ProjectileComp = SpawnedAcorn->FindComponentByClass<UProjectileMovementComponent>())
			{
				ProjectileComp->Velocity = SpawnRotation.Vector() * 1500.0f;
				ProjectileComp->SetVelocityInLocalSpace(FVector::ForwardVector * 1500.0f);
			}
		}
	}
}

void AHavingKeySquirrel::ExtractKey()
{
	if (!bKeyThrown && KeyMeshComponent)
	{
		KeyMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		KeyMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		KeyMeshComponent->SetSimulatePhysics(true);
		KeyMeshComponent->SetNotifyRigidBodyCollision(true); // NOTE: Required for OnHit delegates
		KeyMeshComponent->OnComponentHit.AddDynamic(this, &AHavingKeySquirrel::OnHit);

		FVector ThrowDirection = GetActorForwardVector();
		
		FVector ImpulseForce = ThrowDirection * ThrowStrength + FVector(0.0f, 0.0f, UpwardStrength);
		KeyMeshComponent->AddImpulse(ImpulseForce);

		bKeyThrown = true;
		TimeSinceThrow = 0.0f;

		if (ExtractingKeySound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExtractingKeySound, GetActorLocation());
		}
	}
}

