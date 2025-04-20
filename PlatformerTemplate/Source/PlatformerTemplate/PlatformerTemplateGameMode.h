// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlatformerTemplateGameMode.generated.h"

class AMario64Character;
class APotatoCharacter;
class AFoxCharacter;
class ACheckPoint_Flag;

UCLASS(minimalapi)
class APlatformerTemplateGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APlatformerTemplateGameMode();

	void RespawnPlayer();

	void SetCheckpoint(ACheckPoint_Flag* ToSetFlag, AMario64Character* IndicatorCharacterRef);

	void TeleportCharacter(AMario64Character* ToTeleportCharacter, const FVector& SpawnLocation, const FRotator& SpawnRotration);

protected:
	virtual void BeginPlay() override;

	void SpawnCharacter(FVector SpawnLocation);

public:
	UPROPERTY()
	class ACheckPoint_Flag* LastCheckPoint;

	APotatoCharacter* Potato;

	AFoxCharacter* Fox;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Configs")
	TSubclassOf<APotatoCharacter> PotatoCharacterClass;

	UPROPERTY(EditDefaultsOnly, Category = "Configs")
	TSubclassOf<AFoxCharacter> FoxCharacterClass;

	UPROPERTY()
	AActor* InitCheckpoint;

	UPROPERTY()
	TArray<AActor*> LevelCheckpoints;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* TeleportEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	USoundBase* TeleportSound;
};



