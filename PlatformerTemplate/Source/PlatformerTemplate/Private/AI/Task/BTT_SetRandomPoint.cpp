// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTT_SetRandomPoint.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"

UBTT_SetRandomPoint::UBTT_SetRandomPoint()
{
	NodeName = "Set Random Point";
}

EBTNodeResult::Type UBTT_SetRandomPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}
	ACharacter* ControlledCharacter = Cast<ACharacter>(AIController->GetPawn());
	if (!ControlledCharacter)
	{
		return EBTNodeResult::Failed;
	}

	FVector Origin = ControlledCharacter->GetActorLocation();
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (!NavSys)
		return EBTNodeResult::Failed;

	FNavLocation RandomPoint;

	if (NavSys->GetRandomReachablePointInRadius(
		Origin,
		Radius,
		RandomPoint
	))
	{
		Blackboard->SetValueAsVector(TargetLocationKey, RandomPoint.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
