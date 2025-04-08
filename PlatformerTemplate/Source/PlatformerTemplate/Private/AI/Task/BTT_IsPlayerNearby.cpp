// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTT_IsPlayerNearby.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

UBTT_IsPlayerNearby::UBTT_IsPlayerNearby()
{
	NodeName = "Is Player Nearby";
	PlayerActorKey = "PlayerActor";
}

EBTNodeResult::Type UBTT_IsPlayerNearby::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	AActor* PlayerActor = Cast<AActor>(BlackboardComp->GetValueAsObject(PlayerActorKey));
	if (!PlayerActor)
	{
		return EBTNodeResult::Failed;
	}

	float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), PlayerActor->GetActorLocation());

	if (bShowDebug)
	{
		FColor DebugColor = (Distance <= ProximityThreshold) ? FColor::Green : FColor::Red;
		DrawDebugSphere(AIController->GetWorld(), ControlledPawn->GetActorLocation(), ProximityThreshold, 12, DebugColor, false, 0.5f);
		DrawDebugLine(AIController->GetWorld(), ControlledPawn->GetActorLocation(), PlayerActor->GetActorLocation(), DebugColor, false, 0.5f, 0, 2.0f);
	}

	if (Distance <= ProximityThreshold)
	{
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
