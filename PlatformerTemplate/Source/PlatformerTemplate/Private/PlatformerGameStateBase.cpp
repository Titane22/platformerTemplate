// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformerGameStateBase.h"
#include "Obstacles/HavingKeySquirrel.h"
#include "Kismet/GameplayStatics.h"

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

	if (HavingKeySquirrelInThisLevel)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("HavingKeySquirrel!"));
		if (PlayerScore >= LevelMaxScore)
		{
			UE_LOG(LogTemp, Warning, TEXT("ExtractKey!"));
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("ExtractKey!"));
			HavingKeySquirrelInThisLevel->ExtractKey();
		}
	}	
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Squirrel Found!"));
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Squirrel Not Found!"));
	}
}

void APlatformerGameStateBase::SetLevelMaxScore(int32 MaxScoreToSet)
{
	LevelMaxScore = MaxScoreToSet;
}

void APlatformerGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	AActor* Squirrel = UGameplayStatics::GetActorOfClass(GetWorld(), AHavingKeySquirrel::StaticClass());
	if(Squirrel)
	{
		HavingKeySquirrelInThisLevel = Cast<AHavingKeySquirrel>(Squirrel);
		UE_LOG(LogTemp, Warning, TEXT("Squirrel Found!"));
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Squirrel Found!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Squirrel Not Found!"));
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Squirrel Not Found!"));
	}
}
