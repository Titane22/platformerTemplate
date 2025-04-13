// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ClearFocus.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMERTEMPLATE_API UClearFocus : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UClearFocus();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
