// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SetRandomPoint.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMERTEMPLATE_API UBTT_SetRandomPoint : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_SetRandomPoint();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FName TargetLocationKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Radius = 500.0f;
};
