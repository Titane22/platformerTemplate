// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/PT_Enemy.h"
#include "Enemy_Bull.generated.h"

class UBoxComponent;

/**
 * 
 */
UCLASS()
class PLATFORMERTEMPLATE_API AEnemy_Bull : public APT_Enemy
{
	GENERATED_BODY()
	
public:
	AEnemy_Bull();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* Box;
};
