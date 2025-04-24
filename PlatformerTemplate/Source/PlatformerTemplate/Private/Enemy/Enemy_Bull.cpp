// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy_Bull.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Mario64Character.h"
#include "Character/FoxCharacter.h"
#include "Character/PotatoCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "Obstacles/BlockBase.h"

AEnemy_Bull::AEnemy_Bull()
	:Super()
{
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(RootComponent);
	
	Box->SetCollisionProfileName(TEXT("Trigger")); 
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly); 
	Box->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	if (CapsuleComp)
	{
		CapsuleComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
		CapsuleComp->CanCharacterStepUpOn = ECB_No;
	}

	MaxHealth = 15.0f;
}

void AEnemy_Bull::BeginPlay()
{
	Super::BeginPlay();

	Potato = FindPlayer(APotatoCharacter::StaticClass());
	Fox = FindPlayer(AFoxCharacter::StaticClass());
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemy_Bull::OnHit);
	
	AttackDirection = FVector::ZeroVector;
	StunTimer = 0.0f;
}

void AEnemy_Bull::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (CurrentState)
	{
	case EBullState::Idle:
		HandleIdle();
		break;
	case EBullState::Attack:
		HandleAttack();
		break;
	case EBullState::Stun:
		HandleStun();
		StunTimer += DeltaTime;
		if (StunTimer >= 2.0f) 
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("To Idle"));
			CurrentState = EBullState::Idle;
		}
		break;
	case EBullState::Dead:
		break;
	default:
		break;
	}
}

AMario64Character* AEnemy_Bull::GetTargetPlayer()
{
	if (Potato && Fox)
	{
		float DistanceToPotato = FVector::Dist(GetActorLocation(), Potato->GetActorLocation());
		float DistanceToFox = FVector::Dist(GetActorLocation(), Fox->GetActorLocation());
		
		if (DistanceToPotato <= DistanceToFox)
		{
			UE_LOG(LogTemp, Warning, TEXT("Bull Enemy: Targeting Potato (Distance: %.2f)"), DistanceToPotato);
			return Potato;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Bull Enemy: Targeting Fox (Distance: %.2f)"), DistanceToFox);
			return Fox;
		}
	}
	else if (Potato)
	{
		return Potato;
	}
	else if (Fox)
	{
		return Fox;
	}
	
	return nullptr;
}

AMario64Character* AEnemy_Bull::FindPlayer(TSubclassOf<AMario64Character> ToFindCharacter)
{
	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ToFindCharacter, FoundActors);
	if (FoundActors.Num() > 0)
	{
		AMario64Character* FoundPlayer = Cast<AMario64Character>(FoundActors[0]);
		UE_LOG(LogTemp, Warning, TEXT("Bull Enemy: %s character found in level"), *ToFindCharacter->GetName());
		return FoundPlayer;
	}

	return nullptr;
}

void AEnemy_Bull::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherComp && OtherActor && OtherActor != this)
	{
		if (OtherComp->GetOwner() == this)
		{
			return;
		}
		ECollisionChannel CollisionChannel = OtherComp->GetCollisionObjectType();
		if (RushSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, RushSound, GetActorLocation(), 0.0f);
		}
		AMario64Character* HitPlayer = Cast<AMario64Character>(OtherActor);
		if (HitPlayer && HitPlayer->GetPlayerState() != EActionState::Burrowed)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Current State: %d"), HitPlayer->GetPlayerState()));
			FPointDamageEvent DamageEvent(1.0f, Hit, -FVector::UpVector, nullptr);
			//HitPlayer->TakeDamage(1.0f, DamageEvent, nullptr, this);

			float ForceFactor = 3000.0f;

			FVector DirectionVector = (HitPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			DirectionVector.Z = 0.8f;

			HitPlayer->LaunchCharacter(DirectionVector * ForceFactor, true, true);
		}
		
		ABlockBase* Block = Cast<ABlockBase>(OtherActor);
		if (Block)
		{
			AAIController* AIC = Cast<AAIController>(GetController());
			if (AIC)
			{
				AIC->StopMovement();
				CurrentState = EBullState::Stun;
				UBlackboardComponent* BlackboardComp = AIC->GetBlackboardComponent();
				if (BlackboardComp)
				{
					// TODO: 충격 후 잠시 정지 시간 설정
					// BlackboardComp->SetValueAsFloat(FName("StunTime"), 1.0f);
				}
			}
		}
	}
}

void AEnemy_Bull::HandleIdle()
{
	CurrentState = EBullState::Attack;
	if (CowSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CowSound, GetActorLocation());
	}
	
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->SetFocus(GetTargetPlayer());
	}

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(
		TimerHandle,
		[this]() 
		{
			TargetPlayer = GetTargetPlayer();
			if (TargetPlayer)
			{
				if (RushSound)
				{
					UGameplayStatics::PlaySoundAtLocation(this, RushSound, GetActorLocation());
				}
				AttackDirection = (TargetPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();
				AttackDirection.Z = 0.0f;
			}
		},
		PawingTime,
		false
	);
}

void AEnemy_Bull::HandleStun()
{
	TargetPlayer = nullptr;
	AttackDirection = FVector::ZeroVector;
}

void AEnemy_Bull::HandleAttack()
{
	if (!TargetPlayer)
	{
		return;
	}
	
	FVector ForwardDirection = AttackDirection.IsNearlyZero() ? GetActorForwardVector() : AttackDirection;
	StunTimer = 0.0f;

	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->MaxWalkSpeed = 1500.0f;
		AddMovementInput(ForwardDirection, 1.0f);
		
		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			AIController->StopMovement();
			AIController->ClearFocus(EAIFocusPriority::Gameplay);
		}
	}
}

void AEnemy_Bull::Die_Implementation()
{
	Super::Die_Implementation();

	CurrentState = EBullState::Dead;
}

void AEnemy_Bull::OnDamaged_Implementation()
{
	Super::OnDamaged_Implementation();
	if (CurrentState == EBullState::Attack)
	{
		CurrentState = EBullState::Stun;
		StunTimer = 0.0f;
	}
	if (StunSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, StunSound, GetActorLocation());
	}
}