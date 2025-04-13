// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SetFocus.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMERTEMPLATE_API UBTT_SetFocus : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_SetFocus();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector AttackTargetKey;
};
