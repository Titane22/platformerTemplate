// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FoxCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedActionKeyMapping.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"

AFoxCharacter::AFoxCharacter()
	:Super()
{
	PickingPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Picking Point"));
	PickingPoint->SetupAttachment(RootComponent);
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
		HoldActor();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No Hit Detected"));
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

	DrawDebugCapsule(
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
	);

	if (bHit)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && !HitActor->IsRootComponentStatic() && !HitActor->IsRootComponentStationary())
		{
			if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(HitActor->GetRootComponent()))
			{
				PrimComp->SetSimulatePhysics(false);
				PrimComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				PrimComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

				if (ACharacter* Potato = Cast<ACharacter>(HitActor))
				{
					// 중요: Character 클래스의 경우 무브먼트 컴포넌트의 틱을 비활성화해야 함
					// 무브먼트 컴포넌트는 매 프레임마다 캐릭터의 위치와 회전을 자동으로 계산하는데,
					// 이 계산이 부착된 캐릭터가 마우스 움직임에 따라 회전하는 원인임
					// 이 코드가 없으면 부착된 캐릭터가 마우스 회전에 영향을 받아 계속 회전하게 됨
					if (UCharacterMovementComponent* MovementComp = Potato->GetCharacterMovement())
					{
						MovementComp->SetComponentTickEnabled(false);
					}
				}
				
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
