// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/PT_Enemy.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimInstance.h"

// 생성자
APT_Enemy::APT_Enemy()
{
	// 이 액터가 Tick()을 매 프레임마다 호출하도록 설정
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bRequestedMoveUseAcceleration = true;
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

void APT_Enemy::BeginPlay()
{
	Super::BeginPlay();
	
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void APT_Enemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APT_Enemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APT_Enemy::Die_Implementation()
{
	if (bIsDying)
		return;
		
	bIsDying = true;
	
	if (Controller)
	{
		Controller->UnPossess();
	}
	
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();

	if (DieMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->OnMontageEnded.RemoveDynamic(this, &APT_Enemy::OnDieMontageEnded);
			AnimInstance->OnMontageEnded.AddDynamic(this, &APT_Enemy::OnDieMontageEnded);
			
			float PlayRate = 1.0f;
			AnimInstance->Montage_Play(DieMontage, PlayRate);
			
			if (!AnimInstance->IsAnyMontagePlaying())
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to play death montage!"));
				Destroy();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Death montage playing successfully"));
				
				float MontageLength = DieMontage->GetPlayLength();
				FTimerHandle DestroyTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(
					DestroyTimerHandle,
					this,
					&APT_Enemy::ForceDestroy,
					MontageLength + 0.5f, 
					false
				);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("No AnimInstance found!"));
			Destroy();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No death montage set, destroying immediately"));
		Destroy();
	}
}

// 강제 파괴 함수 추가
void APT_Enemy::ForceDestroy()
{
	UE_LOG(LogTemp, Warning, TEXT("Force destroying enemy after montage timeout"));
	Destroy();
}

void APT_Enemy::OnDamaged_Implementation()
{
}

void APT_Enemy::OnDieMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == DieMontage)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->OnMontageEnded.RemoveDynamic(this, &APT_Enemy::OnDieMontageEnded);
		}
		Destroy();
	}
}

