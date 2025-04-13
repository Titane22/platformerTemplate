// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTT_FindRushTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

UBTT_FindRushTarget::UBTT_FindRushTarget()
{
	NodeName = "Find Rush Target";
}

EBTNodeResult::Type UBTT_FindRushTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (!AIController || !Blackboard)
	{
		return EBTNodeResult::Failed;
	}

	ACharacter* ControlledCharacter = Cast<ACharacter>(AIController->GetPawn());
	if (!ControlledCharacter)
	{
		return EBTNodeResult::Failed;
	}

	const float ExtendedDistance = 3000.0f;
	FVector Direction = ControlledCharacter->GetActorForwardVector();
	
	FVector StartLocation = ControlledCharacter->GetActorLocation();
	FVector EndLocation = StartLocation + (Direction * RaycastDistance);
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(ControlledCharacter);
	
	FHitResult Hit;
	
	if(UKismetSystemLibrary::LineTraceSingle(
		ControlledCharacter->GetWorld(),
		StartLocation,
		EndLocation,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		ActorsToIgnore,
		DrawDebugType,
		Hit,
		true
	))
	{
		FVector FarTarget = Hit.Location + (Direction * ExtendedDistance);
		
		Blackboard->SetValueAsVector(TargetLocationKey, FarTarget);
		return EBTNodeResult::Succeeded;
	}
	else
	{
		FVector FarTarget = StartLocation + (Direction * ExtendedDistance);
		
		Blackboard->SetValueAsVector(TargetLocationKey, FarTarget);
		return EBTNodeResult::Succeeded;
	}
}
