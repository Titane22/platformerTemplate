// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/PT_Enemy.h"
#include "Enemy_Bull.generated.h"

class UBoxComponent;
class AMario64Character;
/**
 * 
 */
UCLASS()
class PLATFORMERTEMPLATE_API AEnemy_Bull : public APT_Enemy
{
	GENERATED_BODY()
	
public:
	AEnemy_Bull();

	AMario64Character* GetTargetPlayer();
	
protected:
	virtual void BeginPlay() override;

	AMario64Character* FindPlayer(TSubclassOf<AMario64Character> ToFindCharacter);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* Box;

	AMario64Character* Potato;

	AMario64Character* Fox;
};
