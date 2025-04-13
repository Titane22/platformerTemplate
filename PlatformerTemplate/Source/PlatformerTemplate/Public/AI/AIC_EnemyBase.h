// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h" 
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISenseConfig_Sight.h"
#include "AIC_EnemyBase.generated.h"

// 전방 선언
class AMario64Character;

UENUM(BlueprintType)
enum class ESensorType : uint8
{
    None     UMETA(DisplayName = "None"),
    Sight    UMETA(DisplayName = "Sight"),
    Hearing  UMETA(DisplayName = "Hearing"),
    Damage   UMETA(DisplayName = "Damage")
};

UENUM(BlueprintType)
enum class EAIState : uint8
{
    Passive			UMETA(DisplayName = "Passive"),
    Attacking		UMETA(DisplayName = "Attacking")
};

/**
 * 
 */
UCLASS()
class PLATFORMERTEMPLATE_API AAIC_EnemyBase : public AAIController
{
	GENERATED_BODY()
public:
    AAIC_EnemyBase();

    virtual void OnPossess(APawn* InPawn);

protected:
    UFUNCTION(BlueprintCallable, Category = "AI|Perception", meta = (BlueprintPure = false))
    bool CanSenseActor(AActor* Actor, ESensorType Sense, FAIStimulus& Stimulus);

    UFUNCTION()
    void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

    virtual void Tick(float DeltaTime) override;

    bool GetRandomPointOnNavMesh(const FVector& Origin, float Radius, FVector& ResultLocation);
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception")
    UAIPerceptionComponent* AIPerception;

    UPROPERTY(EditAnywhere, Category = "AI|Perception")
    UAISenseConfig_Sight* SightConfig;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    UBehaviorTree* BehaviorTree;

    EAIState CurrentState = EAIState::Passive;

    AActor* LastestSensedActor;

    UPROPERTY()
    UBehaviorTreeComponent* BehaviorTreeComponent;

    UPROPERTY()
    UBlackboardComponent* BlackboardComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    FName TargetLocationKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    FName AIStateKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    FName AttackTargetKey;
};
