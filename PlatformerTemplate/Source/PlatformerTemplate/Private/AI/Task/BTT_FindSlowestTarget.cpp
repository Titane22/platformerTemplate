// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTT_FindSlowestTarget.h"
#include "AIController.h"
#include "Mario64Character.h"
#include "Enemy/Enemy_Bull.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_FindSlowestTarget::UBTT_FindSlowestTarget()
{
	NodeName = "Find Nearest Target";
}

EBTNodeResult::Type UBTT_FindSlowestTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

    ACharacter* ControlledCharacter = Cast<ACharacter>(AIController->GetPawn());
    if (!ControlledCharacter)
    {
        return EBTNodeResult::Failed;
    }
    AEnemy_Bull* Bull = Cast<AEnemy_Bull>(ControlledCharacter);
    if(!Bull)
    {
        return EBTNodeResult::Failed;
    }

    BlackboardComp->SetValueAsObject(AttackTargetKey.SelectedKeyName, Bull->GetTargetPlayer());

	return EBTNodeResult::Succeeded;
}
