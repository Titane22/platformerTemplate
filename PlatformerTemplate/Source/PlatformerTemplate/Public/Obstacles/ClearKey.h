// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "ClearKey.generated.h"

class AMario64Character;
class UStaticMeshComponent;

UCLASS()
class PLATFORMERTEMPLATE_API AClearKey : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AClearKey();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// 플레이어 캐릭터가 이 열쇠를 가지고 있는지 확인하는 함수
	UFUNCTION(BlueprintCallable, Category = "Key")
	bool IsOwnedByPlayer() const { return bIsOwned; }

	void ResetKey();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* KeyMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UCapsuleComponent* CheckVolume;

	UPROPERTY()
	AMario64Character* PlayerRef;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Key")
	USoundBase* GettingKeySound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Key")
	bool bIsOwned = false;
	
	UPROPERTY()
	FTransform OriginalTransform;
};
