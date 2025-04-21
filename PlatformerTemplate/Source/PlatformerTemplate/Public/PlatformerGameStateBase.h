// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PlatformerGameStateBase.generated.h"

class AHavingKeySquirrel;

/**
 * 
 */
UCLASS()
class PLATFORMERTEMPLATE_API APlatformerGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	APlatformerGameStateBase();

	void AddScore(int32 ScoreToAdd);

	void ResetScore() { PlayerScore = 0.0f; }

	void SetLevelMaxScore(int32 MaxScoreToSet);

protected:
	UPROPERTY(BlueprintReadWrite, Category="Scoring")
	float PlayerScore = 0.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Scoring")
	float LevelMaxScore = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Scoring")
	AHavingKeySquirrel* HavingKeySquirrelInThisLevel;
};
