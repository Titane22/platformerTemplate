// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy_Wolf.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Mario64Character.h"
#include "AIController.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

AEnemy_Wolf::AEnemy_Wolf()
	:Super()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->GravityScale = 3.0f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	
}

void AEnemy_Wolf::Die_Implementation()
{
	DieMontage = DieMont;
	Super::Die_Implementation();
	if (DeadSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeadSound, GetActorLocation());
	}
}

void AEnemy_Wolf::OnDamaged_Implementation()
{
	Super::OnDamaged_Implementation();

}

void AEnemy_Wolf::BeginPlay()
{
	Super::BeginPlay();

	if (UCapsuleComponent* RootComp = GetCapsuleComponent())
	{
		RootComp->OnComponentHit.AddDynamic(this, &AEnemy_Wolf::OnHit);
	}
}

void AEnemy_Wolf::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this)
	{
		if (AMario64Character* Player = Cast<AMario64Character>(OtherActor))
		{
			FPointDamageEvent DamageEvent(1.0f, Hit, -FVector::UpVector, nullptr);
			Player->TakeDamage(1.0f, DamageEvent, nullptr, this);
			
			float ForceFactor = 1000.0f;
			
			FVector DirectionVector = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			DirectionVector.Z = 0.3f; 
			
			Player->LaunchCharacter(DirectionVector * ForceFactor, true, true);
			LaunchCharacter(-DirectionVector * ForceFactor * 0.7f, true, true);
			
			// TODO: 충돌 효과 재생 (소리, 파티클 등)
		}
		else
		{
			ECollisionChannel CollisionChannel = OtherComp->GetCollisionObjectType();
			
			if (CollisionChannel == ECC_WorldStatic || CollisionChannel == ECC_WorldDynamic)
			{
				if (AAIController* AIC = Cast<AAIController>(GetController()))
				{
					AIC->StopMovement();
				}
			}
		}
	}
}
