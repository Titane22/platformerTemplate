// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformerGameStateBase.h"

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
}

void APlatformerGameStateBase::SetLevelMaxScore(int32 MaxScoreToSet)
{
	LevelMaxScore = MaxScoreToSet;
}
