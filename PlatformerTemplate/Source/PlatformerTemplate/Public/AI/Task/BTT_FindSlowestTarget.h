// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FindSlowestTarget.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMERTEMPLATE_API UBTT_FindSlowestTarget : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_FindSlowestTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector AttackTargetKey;
};
