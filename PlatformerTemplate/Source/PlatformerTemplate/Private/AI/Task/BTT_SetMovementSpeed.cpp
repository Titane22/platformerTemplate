// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTT_SetMovementSpeed.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"

UBTT_SetMovementSpeed::UBTT_SetMovementSpeed()
{
	NodeName = "Set Movement Speed";
}

EBTNodeResult::Type UBTT_SetMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}
	ACharacter* ControlledCharacter = Cast<ACharacter>(AIController->GetPawn());
	if (!ControlledCharacter)
	{
		return EBTNodeResult::Failed;
	}

	UCharacterMovementComponent* CMC = ControlledCharacter->GetCharacterMovement();
	if (!CMC)
	{
		return EBTNodeResult::Failed;
	}
	CMC->MaxWalkSpeed = ToSetSpeed;

	return EBTNodeResult::Succeeded;
}
