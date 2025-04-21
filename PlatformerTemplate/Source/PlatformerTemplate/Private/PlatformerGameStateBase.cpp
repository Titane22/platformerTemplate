// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformerGameStateBase.h"
#include "Obstacles/HavingKeySquirrel.h"
APlatformerGameStateBase::APlatformerGameStateBase()
	:Super()
{
}

void APlatformerGameStateBase::AddScore(int32 ScoreToAdd)
{
	if (PlayerScore < LevelMaxScore)
	{
		PlayerScore += ScoreToAdd;
	}

	if (HavingKeySquirrelInThisLevel && PlayerScore >= LevelMaxScore)
	{
		HavingKeySquirrelInThisLevel->ExtractKey();
	}
}

void APlatformerGameStateBase::SetLevelMaxScore(int32 MaxScoreToSet)
{
	LevelMaxScore = MaxScoreToSet;
}
