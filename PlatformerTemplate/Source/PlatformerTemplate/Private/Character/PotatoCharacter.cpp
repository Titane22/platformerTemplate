// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PotatoCharacter.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedActionKeyMapping.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/DamageEvents.h"
#include "Enemy/PT_Enemy.h"

APotatoCharacter::APotatoCharacter()
	:Super()
{
	BurrowBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Burrow Collision"));
	BurrowBox->SetupAttachment(RootComponent);
	BurrowBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BurrowBox->SetRelativeLocation(FVector(0.0f, 0.0f, -85.0f));

	//BurrowTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Burrow Timeline"));
	//UnburrowTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Unburrow Timeline"));
}

void APotatoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(BurrowAction, ETriggerEvent::Triggered, this, &APotatoCharacter::ToogleBurrow);
	}
}

void APotatoCharacter::BeginPlay()
{
	Super::BeginPlay();

	OrgCollisionEnabled = GetCapsuleComponent()->GetCollisionEnabled();
	SaveOriginalCollisionResponses();
	EnableCamera(true);
	/*if (BurrowCurve)
	{
		FOnTimelineFloat BurrowCallback;
		BurrowCallback.BindUFunction(this, FName("UpdateBurrowHeight"));
		BurrowTimeline->AddInterpFloat(BurrowCurve, BurrowCallback);
	}

	if (UnburrowCurve)
	{
		FOnTimelineFloat UnburrowCallback;
		UnburrowCallback.BindUFunction(this, FName("UpdateUnburrowHeight"));
		UnburrowTimeline->AddInterpFloat(UnburrowCurve, UnburrowCallback);
	}*/
}

void APotatoCharacter::SaveOriginalCollisionResponses()
{
	for (int32 i = 0; i < COL_MAX; i++)
	{
		ECollisionChannel Channel = static_cast<ECollisionChannel>(i);
		OriginalCollisionResponses[i] = GetCapsuleComponent()->GetCollisionResponseToChannel(Channel);
	}
}

void APotatoCharacter::ImpulseForceOnPotatoHead()
{
	if (OnPotatoHead.Num() == 0)
		return;

	for (AActor* Actor : OnPotatoHead)
	{
		if (!Actor)
			continue;
		if (AMario64Character* Fox = Cast<AMario64Character>(Actor))
		{
			Fox->LaunchCharacter(LaunchVelocity, false, false);
		}
		else if(APT_Enemy* CharacterOnHead = Cast<APT_Enemy>(Actor))
		{
			FHitResult TempHit;
			TempHit.ImpactPoint = Actor->GetActorLocation();
			TempHit.ImpactNormal = FVector(0, 0, 1);
			TempHit.Location = GetActorLocation();
			
			FPointDamageEvent DamageEvent(5.0f, TempHit, -FVector::UpVector, nullptr);
			CharacterOnHead->TakeDamage(5.0f, DamageEvent, nullptr, this);
			CharacterOnHead->LaunchCharacter(LaunchVelocity, false, false);
		}
	}
}

void APotatoCharacter::OnBurrowBoxHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor)
		return;

	if (Hit.ImpactPoint.Z > BurrowBox->GetComponentLocation().Z)
	{
		OnPotatoHead.AddUnique(OtherActor);
		
		Unburrow();
	}
}

void APotatoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CurrentState == EActionState::Burrowed && OnPotatoHead.Num() > 0)
	{
		for (int32 i = OnPotatoHead.Num() - 1; i >= 0; --i)
		{
			AActor* Actor = OnPotatoHead[i];
			
			if (!IsValid(Actor))
			{
				OnPotatoHead.RemoveAt(i);
				continue;
			}
			
			float Distance = FVector::Dist(GetActorLocation(), Actor->GetActorLocation());
			const float MaxDistance = 100.0f; 
			
			if (Distance > MaxDistance)
			{
				OnPotatoHead.RemoveAt(i);
			}
		}
	}
}

bool APotatoCharacter::CheckPlatGround()
{
	FVector StartLocation = LegPoint->GetComponentLocation();
	FVector EndLocation = StartLocation - FVector(0.0f, 0.0f, 1000.0f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(
		Hit,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		QueryParams
	))
	{
		FVector SurfaceNormal = Hit.Normal;
		if (FMath::Abs(SurfaceNormal.Z) >= 0.9f)
		{
			return true;
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("This is not Plat Ground"));
	return false;
}

void APotatoCharacter::ToogleBurrow(const FInputActionValue& Value)
{
	if (CurrentState != EActionState::Idle && CurrentState != EActionState::Burrowed)
		return;
	if (!CheckPlatGround())
		return;
	bIsBurrowed = !bIsBurrowed;
	if (bIsBurrowed)
	{
		Burrow();
	}
	else
	{
		Unburrow();
	}
}

void APotatoCharacter::Burrow()
{
	SetState(EActionState::Burrowed);
	if (BurrowBox)
	{
		FVector BoxOrigin = BurrowBox->GetComponentLocation();
		FVector BoxExtent = BurrowBox->GetScaledBoxExtent();

		//DrawDebugBox(GetWorld(), BoxOrigin, BoxExtent, FColor::Yellow, false, 3.0f);
		if (BurrowMontage)
		{
			PlayAnimMontage(BurrowMontage);
		}
		BurrowBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		BurrowBox->OnComponentHit.AddDynamic(this, &APotatoCharacter::OnBurrowBoxHit);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		// TODO: Need to Check, if Other Actor can move up
		/*GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Overlap);*/
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
}

void APotatoCharacter::Unburrow()
{
	
	if (BurrowMontage)
	{
		StopAnimMontage(BurrowMontage);
	}
	
	if (BurrowBox)
	{
		BurrowBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BurrowBox->OnComponentHit.RemoveDynamic(this, &APotatoCharacter::OnBurrowBoxHit);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetCapsuleComponent()->SetCollisionEnabled(OrgCollisionEnabled);

		if (OnPotatoHead.Num() > 0)
		{
			ImpulseForceOnPotatoHead();
		}
		else
		{
			FVector ImpulsePower = FVector(0.0f, 0.0f, 800.0f);
			LaunchCharacter(ImpulsePower, false, false);
		}
		
		for (int32 i = 0; i < COL_MAX; i++)
		{
			ECollisionChannel Channel = static_cast<ECollisionChannel>(i);
			GetCapsuleComponent()->SetCollisionResponseToChannel(Channel, OriginalCollisionResponses[i]);
		}
	}
	//SetState(EActionState::Idle);

}
