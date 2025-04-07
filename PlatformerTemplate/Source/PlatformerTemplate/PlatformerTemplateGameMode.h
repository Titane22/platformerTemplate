// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlatformerTemplateGameMode.generated.h"

class AMario64Character;
class APotatoCharacter;
class AFoxCharacter;

UCLASS(minimalapi)
class APlatformerTemplateGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APlatformerTemplateGameMode();

	void RespawnPlayer();
protected:
	virtual void BeginPlay() override;

	void SpawnCharacter(FVector SpawnLocation);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoints")
	class ACheckPoint_Flag* LastCheckPoint;

	APotatoCharacter* Potato;

	AFoxCharacter* Fox;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Configs")
	TSubclassOf<APotatoCharacter> PotatoCharacterClass;

	UPROPERTY(EditDefaultsOnly, Category = "Configs")
	TSubclassOf<AFoxCharacter> FoxCharacterClass;
};



