// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_CheckIfStopped.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMERTEMPLATE_API UBTD_CheckIfStopped : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTD_CheckIfStopped();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OtherComp, uint8* NodeMemory) const override;
};
