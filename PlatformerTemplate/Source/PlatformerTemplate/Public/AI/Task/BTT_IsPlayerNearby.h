// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_IsPlayerNearby.generated.h"

/**
 * 비헤이비어 트리 태스크: 플레이어가 AI 근처에 있는지 확인합니다.
 */
UCLASS()
class PLATFORMERTEMPLATE_API UBTT_IsPlayerNearby : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_IsPlayerNearby();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	// 블랙보드에서 플레이어 액터를 가져올 키 이름
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName PlayerActorKey;
	
	// 근접 확인을 위한 거리 임계값
	UPROPERTY(EditAnywhere, Category = "AI", meta = (ClampMin = "0.0"))
	float ProximityThreshold = 200.0f;
	
	// 디버그 시각화 활성화 여부
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShowDebug = false;
};
