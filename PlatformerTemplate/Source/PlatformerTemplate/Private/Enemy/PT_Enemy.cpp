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
			// 디버그 정보 출력
			UE_LOG(LogTemp, Warning, TEXT("Playing death montage: %s, Duration: %f"), 
				*DieMontage->GetName(), DieMontage->GetPlayLength());
			
			// 기존 바인딩된 콜백이 있다면 제거
			AnimInstance->OnMontageEnded.RemoveDynamic(this, &APT_Enemy::OnDieMontageEnded);
			
			// 새로 콜백 바인딩
			AnimInstance->OnMontageEnded.AddDynamic(this, &APT_Enemy::OnDieMontageEnded);
			
			// 몽타주 재생 - 명시적 재생 속도 (1.0) 지정
			float PlayRate = 1.0f;
			AnimInstance->Montage_Play(DieMontage, PlayRate);
			
			// 몽타주가 실행 중인지 확인
			if (!AnimInstance->IsAnyMontagePlaying())
			{
				// 몽타주가 재생되지 않으면 바로 파괴
				UE_LOG(LogTemp, Error, TEXT("Failed to play death montage!"));
				Destroy();
			}
			else
			{
				// 몽타주가 제대로 재생되었다면 몽타주 끝날 때 파괴되도록 함
				UE_LOG(LogTemp, Warning, TEXT("Death montage playing successfully"));
				
				// 몽타주가 멈춰있는 경우를 대비한 안전장치
				// 몽타주 길이보다 약간 더 긴 시간 후에 자동으로 파괴
				float MontageLength = DieMontage->GetPlayLength();
				FTimerHandle DestroyTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(
					DestroyTimerHandle,
					this,
					&APT_Enemy::ForceDestroy,
					MontageLength + 0.5f, // 0.5초 더 여유를 둠
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

