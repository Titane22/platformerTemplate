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
		CapsuleComp->OnComponentHit.AddDynamic(this, &AEnemy_Bull::OnHit);
	}
}

void AEnemy_Bull::BeginPlay()
{
	Super::BeginPlay();

	Potato = FindPlayer(APotatoCharacter::StaticClass());
	Fox = FindPlayer(AFoxCharacter::StaticClass());
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
		
		// WorldStatic(벽, 바닥 등)이나 WorldDynamic(움직이는 물체)와 충돌한 경우
		if (CollisionChannel == ECC_WorldStatic || CollisionChannel == ECC_WorldDynamic)
		{
			AAIController* AIC = Cast<AAIController>(GetController());
			if (AIC)
			{
				AIC->StopMovement();
				
				UBlackboardComponent* BlackboardComp = AIC->GetBlackboardComponent();
				if (BlackboardComp)
				{
					// TODO: 충격 후 잠시 정지 시간 설정
					// BlackboardComp->SetValueAsFloat(FName("StunTime"), 1.0f);
				}
			}
		}
		
		AMario64Character* HitPlayer = Cast<AMario64Character>(OtherActor);
		if (HitPlayer)
		{
			FPointDamageEvent DamageEvent(1.0f, Hit, -FVector::UpVector, nullptr);
			HitPlayer->TakeDamage(1.0f, DamageEvent, nullptr, this);

			float ForceFactor = 3000.0f;

			FVector DirectionVector = (HitPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			DirectionVector.Z = 0.8f;

			HitPlayer->LaunchCharacter(DirectionVector * ForceFactor, true, true);
			LaunchCharacter(-DirectionVector * ForceFactor * 0.7f, true, true);
			// TODO: 플레이어 데미지 처리 등의 로직은 여기에 구현
		}
	}
}
