// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIC_PlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Mario64Character.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

AAIC_PlayerBase::AAIC_PlayerBase()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	
	PrimaryActorTick.bCanEverTick = true;
}

void AAIC_PlayerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	PlayerCharRef = Cast<AMario64Character>(InPawn);

	// 블랙보드와 비헤이비어 트리 초기화
	if (BehaviorTree && BlackboardComponent)
	{
		BlackboardComponent->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		BehaviorTreeComponent->StartTree(*BehaviorTree);
		
		// 플레이어 찾기
		AMario64Character* PlayerCharacter = FindPlayerCharacter();
		if (PlayerCharacter)
		{
			BlackboardComponent->SetValueAsObject(PlayerKey, PlayerCharacter);
		}
	}
}

void AAIC_PlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 플레이어가 없으면 다시 찾기
	if (!BlackboardComponent->GetValueAsObject(PlayerKey))
	{
		if (AMario64Character* PlayerCharacter = FindPlayerCharacter())
		{
			BlackboardComponent->SetValueAsObject(PlayerKey, PlayerCharacter);
		}
	}
	else
	{
		if (AMario64Character* PlayerCharacter = FindPlayerCharacter())
		{
			//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("PlayerCharacter: %s"), *PlayerCharacter->GetName()));
			BlackboardComponent->SetValueAsVector(TargetLocationKey, PlayerCharacter->GetActorLocation());
		}
	}

}

AMario64Character* AAIC_PlayerBase::FindPlayerCharacter() const
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		return Cast<AMario64Character>(PC->GetPawn());
	}
	return nullptr;
}

void AAIC_PlayerBase::MoveWithoutNavMesh(UNavigationSystemV1* NavSys, FVector TargetLocation, APawn* ControlledPawn)
{
	// 비헤이비어 트리 사용 시 이동 (무시 조건)
	FVector Direction = (TargetLocation - ControlledPawn->GetActorLocation()).GetSafeNormal();
	AMario64Character* AICharacter = Cast<AMario64Character>(ControlledPawn);

	if (AICharacter)
	{
		// 캐릭터 참조
		FRotator TargetRotation = Direction.Rotation();
		AICharacter->SetActorRotation(FMath::RInterpTo(
			AICharacter->GetActorRotation(),
			FRotator(0, TargetRotation.Yaw, 0),
			GetWorld()->GetDeltaSeconds(),
			5.0f
		));

		// 캐릭터 이동
		UCharacterMovementComponent* MovementComponent = AICharacter->GetCharacterMovement();
		if (MovementComponent)
		{
			// 캐릭터 가속 설정
			float DistanceToPlayer = FVector::Dist(AICharacter->GetActorLocation(), PlayerCharRef->GetActorLocation());
			float FollowSpeed = 230.0f; // 기본 추적

			// 캐릭터 가속 설정
			if (DistanceToPlayer < 200.0f)
			{
				FollowSpeed *= 0.5f;
			}
			// 캐릭터 감속 설정
			else if (DistanceToPlayer > 500.0f)
			{
				FollowSpeed *= 1.5f;
			}

			MovementComponent->MaxWalkSpeed = FollowSpeed;

			// 캐릭터 이동
			MovementComponent->AddInputVector(Direction);

			// 캐릭터 높이 차이 고려 추가
			float HeightDifference = PlayerCharRef->GetActorLocation().Z - AICharacter->GetActorLocation().Z;
			if (HeightDifference > 50.0f && MovementComponent->IsMovingOnGround())
			{
				AICharacter->Jump();
			}
		}
	}
}

