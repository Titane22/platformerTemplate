// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/ClearFocus.h"
#include "AIController.h"

UClearFocus::UClearFocus()
{
	NodeName = "ClearFocus";
}

EBTNodeResult::Type UClearFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }
    AIController->ClearFocus(EAIFocusPriority::Gameplay);
    return EBTNodeResult::Succeeded;
}
