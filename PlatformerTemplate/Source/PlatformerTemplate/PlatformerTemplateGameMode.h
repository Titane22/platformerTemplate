// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlatformerTemplateGameMode.generated.h"

class AMario64Character;

UCLASS(minimalapi)
class APlatformerTemplateGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APlatformerTemplateGameMode();

	void RespawnPlayer();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoints")
	class ACheckPoint_Flag* LastCheckPoint;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs")
	TSubclassOf<AMario64Character> PlayerClassRef;
};



