// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator/BTD_CheckIfStopped.h"
#include "AIController.h"
#include "Mario64Character.h"

UBTD_CheckIfStopped::UBTD_CheckIfStopped()
{
	NodeName = "Check If Stopped";
}

bool UBTD_CheckIfStopped::CalculateRawConditionValue(UBehaviorTreeComponent& OtherComp, uint8* NodeMemory) const
{
	AAIController* AIController = OtherComp.GetAIOwner();
	if (!AIController)
	{
		return false;
	}

	AMario64Character* Partner = Cast<AMario64Character>(AIController->GetPawn());
	if (!Partner)
	{
		return false;
	}
	return !Partner->IsStopFromPartner();
}
