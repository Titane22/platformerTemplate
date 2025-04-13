// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIC_EnemyBase.h"
#include "Mario64Character.h"
#include "NavigationSystem.h"

AAIC_EnemyBase::AAIC_EnemyBase()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*AIPerception);

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SightConfig->SightRadius = 1500.0f;
	SightConfig->LoseSightRadius = 2000.0f;
	SightConfig->PeripheralVisionAngleDegrees = 60.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->AutoSuccessRangeFromLastSeenLocation = -1.0f;
	
	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AAIC_EnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState == EAIState::Attacking)
	{
		if (LastestSensedActor)
		{
			AMario64Character* Player = Cast<AMario64Character>(LastestSensedActor);
			if (Player)
			{
				FAIStimulus AIStimulus;
				if (CanSenseActor(Player, ESensorType::Sight, AIStimulus))
				{
					BlackboardComponent->SetValueAsEnum(AIStateKey, static_cast<uint8>(EAIState::Attacking));
					BlackboardComponent->SetValueAsObject(AttackTargetKey, Player);
				}
				else
				{
					BlackboardComponent->SetValueAsEnum(AIStateKey, static_cast<uint8>(EAIState::Passive));
				}
			}
		}
	}
}

bool AAIC_EnemyBase::GetRandomPointOnNavMesh(const FVector& Origin, float Radius, FVector& ResultLocation)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (!NavSys)
		return false;

	FNavLocation RandomPoint;

	if (NavSys->GetRandomReachablePointInRadius(
		Origin,
		Radius,
		RandomPoint
	))
	{
		ResultLocation = RandomPoint.Location;
		return true;
	}
	return false;
}

void AAIC_EnemyBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (BehaviorTree && BehaviorTree->BlackboardAsset)
	{
		if (UseBlackboard(BehaviorTree->BlackboardAsset, BlackboardComponent))
		{
			AIPerception->OnPerceptionUpdated.AddDynamic(this, &AAIC_EnemyBase::OnPerceptionUpdated);
			RunBehaviorTree(BehaviorTree);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to initialize blackboard!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BehaviorTree has no BlackboardAsset!"));
	}
}

bool AAIC_EnemyBase::CanSenseActor(AActor* Actor, ESensorType Sense, FAIStimulus& Stimulus)
{
	if (!Actor || !AIPerception)
		return false;
	
	FActorPerceptionBlueprintInfo info;
	AIPerception->GetActorsPerception(Actor, info);

	for (const FAIStimulus stimulusItem : info.LastSensedStimuli)
	{
		TSubclassOf<UAISense> SenseClass = UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), stimulusItem);

		if (SenseClass == UAISense_Sight::StaticClass() && Sense == ESensorType::Sight)
		{
			Stimulus = stimulusItem;
			return stimulusItem.WasSuccessfullySensed();
		}
	}

	return false;
}

void AAIC_EnemyBase::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	if (!Blackboard)
	{
		UE_LOG(LogTemp, Warning, TEXT("Blackboard is null in OnPerceptionUpdated"));
		return;
	}
	
	for (AActor* Actor : UpdatedActors)
	{
		AMario64Character* Player = Cast<AMario64Character>(Actor);
		if (!Player)
			continue;
		LastestSensedActor = Player;
		CurrentState = EAIState::Attacking;

		FAIStimulus AIStimulus;
		if (CanSenseActor(Actor, ESensorType::Sight, AIStimulus))
		{
			BlackboardComponent->SetValueAsEnum(AIStateKey, static_cast<uint8>(EAIState::Attacking));
			BlackboardComponent->SetValueAsObject(AttackTargetKey, Player);
			break;
		}
	}
}