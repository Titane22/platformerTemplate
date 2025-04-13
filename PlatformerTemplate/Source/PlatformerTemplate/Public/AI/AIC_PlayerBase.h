// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h" 
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "AIC_PlayerBase.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMERTEMPLATE_API AAIC_PlayerBase : public AAIController
{
	GENERATED_BODY()
	
public:
	AAIC_PlayerBase();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION()
	class AMario64Character* FindPlayerCharacter() const;

	void MoveWithoutNavMesh(UNavigationSystemV1* NavSys, FVector TargetLocation, APawn* ControlledPawn);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;

	UPROPERTY()
	UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY()
	UBlackboardComponent* BlackboardComponent;


	UPROPERTY()
	class AMario64Character* PlayerCharRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FName TargetLocationKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FName PlayerKey;
};
