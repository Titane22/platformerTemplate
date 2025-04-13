// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Kismet/KismetSystemLibrary.h"  // EDrawDebugTrace가 여기에 정의되어 있음
#include "BTT_FindRushTarget.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMERTEMPLATE_API UBTT_FindRushTarget : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_FindRushTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Rush")
	float RaycastDistance = 1000.0f;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName TargetLocationKey = "TargetLocation";
	
	UPROPERTY(EditAnywhere, Category = "Debug")
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType = EDrawDebugTrace::None;
};
