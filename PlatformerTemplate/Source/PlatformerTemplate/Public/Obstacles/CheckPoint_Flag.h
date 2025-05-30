// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "CheckPoint_Flag.generated.h"

class APT_Enemy;

UCLASS()
class PLATFORMERTEMPLATE_API ACheckPoint_Flag : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckPoint_Flag();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int32 GetCheckpointOrder() { return CheckpointOrder; }
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* FlagMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* BoxCollision;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* CheckpointSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoints")
	int32 CheckpointOrder = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoints")
	bool bIsBossRoom = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	APT_Enemy* BossRef;
};
