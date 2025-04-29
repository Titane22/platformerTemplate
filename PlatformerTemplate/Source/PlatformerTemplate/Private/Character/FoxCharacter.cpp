// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FoxCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedActionKeyMapping.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Character/PotatoCharacter.h"

AFoxCharacter::AFoxCharacter()
	:Super()
{
	PickingPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Picking Point"));
	PickingPoint->SetupAttachment(RootComponent);
}

void AFoxCharacter::BeginPlay()
{
	Super::BeginPlay();

	OriginSprintSpeed = SprintMaxSpeed;
	OriginWalkSpeed = WalkMaxSpeed;
}

void AFoxCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Triggered, this, &AFoxCharacter::Throw);
	}
}

void AFoxCharacter::Throw()
{
	if (!PartnerRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("AFoxCharacter::ThrowPotato() PartnerRef is NULL"));
		return;
	}
	if (!bIsTakingPotato)
	{
		// 이미 액터를 들고 있다면 던짐
		if (bIsHoldingActor && HeldActor)
		{
			ThrowHeldActor();
		}
		else
		{
			// 아니면 새로 액터를 잡음
			HoldActor();
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No Hit Detected"));
	}
}

// 들고 있는 액터를 전방으로 던지는 함수
void AFoxCharacter::ThrowHeldActor()
{
	if (!HeldActor)
	{
		return;
	}

	FVector ThrowDirection = GetActorForwardVector();
	ThrowDirection.Normalize();

	FVector LaunchVelocity = ThrowDirection * ThrowForce + FVector(0, 0, ThrowUpwardForce);
	HeldActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	AMario64Character* MarioCharacter = Cast<AMario64Character>(HeldActor);
	if (MarioCharacter)
	{
		if (UCharacterMovementComponent* MovementComp = MarioCharacter->GetCharacterMovement())
		{
			MovementComp->SetComponentTickEnabled(true);
			MovementComp->SetMovementMode(EMovementMode::MOVE_Falling);
			// 캐릭터가 날아가는 동안 회전하지 않도록 설정
			MovementComp->bOrientRotationToMovement = false;
			SprintMaxSpeed = OriginSprintSpeed;
			WalkMaxSpeed = OriginWalkSpeed;
		}
		
		if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(MarioCharacter->GetRootComponent()))
		{
			PrimComp->OnComponentHit.RemoveDynamic(this, &AFoxCharacter::OnThrownCharacterHit);
			PrimComp->OnComponentHit.AddDynamic(this, &AFoxCharacter::OnThrownCharacterHit);
			
			// 충돌 활성화
			PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			PrimComp->SetCollisionResponseToAllChannels(ECR_Block);
		}
		
		// LaunchCharacter 함수를 사용하여 캐릭터 발사 (물리 적용)
		MarioCharacter->LaunchCharacter(LaunchVelocity, true, true);
	}
	else
	{
		if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(HeldActor->GetRootComponent()))
		{
			PrimComp->SetSimulatePhysics(true);
			PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			PrimComp->SetCollisionResponseToAllChannels(ECR_Block);
			PrimComp->AddImpulse(LaunchVelocity, NAME_None, true);
		}
	}

	bIsHoldingActor = false;
	HeldActor = nullptr;
	CurrentState = EActionState::Idle;

}

void AFoxCharacter::OnThrownCharacterHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
										UPrimitiveComponent* OtherComp, FVector NormalImpulse, 
										const FHitResult& Hit)
{
	AMario64Character* MarioCharacter = Cast<AMario64Character>(HitComponent->GetOwner());
	if (!MarioCharacter)
	{
		return;
	}
	
	if (Hit.Normal.Z > 0.7f)  
	{
		if (UCharacterMovementComponent* MovementComp = MarioCharacter->GetCharacterMovement())
		{
			// 걷기 모드로 전환
			MovementComp->SetMovementMode(EMovementMode::MOVE_Walking);
			MovementComp->bOrientRotationToMovement = true;
			MovementComp->Velocity = FVector::ZeroVector;
		}
		
		FRotator UpRotation = FRotator(0.0f, MarioCharacter->GetActorRotation().Yaw, 0.0f);
		MarioCharacter->SetActorRotation(UpRotation);
		HitComponent->OnComponentHit.RemoveDynamic(this, &AFoxCharacter::OnThrownCharacterHit);
		TagCharacter();
	}
}

void AFoxCharacter::HoldActor()
{
	FVector Direction = GetActorRotation().Vector();
	Direction.Normalize();

	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + Direction * 150;
	float CapusleRadius = 20.0f;
	float CapsuleHalfHeight = 30.0f;

	FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CapusleRadius, CapsuleHalfHeight);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FHitResult Hit;
	/*if (GetWorld()->LineTraceSingleByChannel(
	Hit,
	StartLocation,
	EndLocation,
	ECC_Pawn,
	QueryParams
	))
	{
	if (Hit.GetActor())
	{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString::Printf(TEXT("Hit Actor: %s"), *Hit.GetActor()->GetName()));
	}

	}
	else
	{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Hit Actor is NULL")));
	}*/
	bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		StartLocation,
		EndLocation,
		FQuat::Identity,
		ECC_Pawn,
		CapsuleShape,
		QueryParams
	);

	/*DrawDebugCapsule(
		GetWorld(),
		StartLocation + (EndLocation - StartLocation) * 0.5f,
		CapsuleHalfHeight,
		CapusleRadius,
		FQuat::Identity,
		bHit ? FColor::Green : FColor::Red,
		false,
		5.0f,
		0,
		2.0f
	);*/

	if (bHit)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && !HitActor->IsRootComponentStatic() && !HitActor->IsRootComponentStationary())
		{
			if (APotatoCharacter* Potato = Cast<APotatoCharacter>(HitActor))
			{
				// 중요: Character 클래스의 경우 무브먼트 컴포넌트의 틱을 비활성화해야 함
				// 무브먼트 컴포넌트는 매 프레임마다 캐릭터의 위치와 회전을 자동으로 계산하는데,
				// 이 계산이 부착된 캐릭터가 마우스 움직임에 따라 회전하는 원인임
				// 이 코드가 없으면 부착된 캐릭터가 마우스 회전에 영향을 받아 계속 회전하게 됨
				if (Potato->GetPlayerState() != EActionState::Idle)
				{
					return;
				}
				SprintMaxSpeed = HoldingSprintSpeed;
				WalkMaxSpeed = HoldingWalkSpeed;
				GetCharacterMovement()->MaxWalkSpeed = WalkMaxSpeed;
				CurrentState = EActionState::Helding;

				if (UCharacterMovementComponent* MovementComp = Potato->GetCharacterMovement())
				{
					MovementComp->SetComponentTickEnabled(false);
				}
				if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(HitActor->GetRootComponent()))
				{
					PrimComp->SetSimulatePhysics(false);
					PrimComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					PrimComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

					HitActor->AttachToComponent(
						PickingPoint,
						FAttachmentTransformRules::SnapToTargetNotIncludingScale
					);

					bIsHoldingActor = true;
					HeldActor = HitActor;
				}
			}
			
		}
	}
}
