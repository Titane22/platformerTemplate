// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlatformerTemplateGameMode.h"
#include "PlatformerTemplateCharacter.h"
#include "Public/Mario64Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Public/Obstacles/CheckPoint_Flag.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

APlatformerTemplateGameMode::APlatformerTemplateGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/Character/BP_PotatoCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
		PlayerClassRef = PlayerPawnBPClass.Class;
	}
}

void APlatformerTemplateGameMode::RespawnPlayer()
{
	if (!PlayerClassRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerClassRef is Null in APlatformerTemplateGameMode::RespawnPlayer()"));
		return;
	}

	if (ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		PlayerCharacter->Destroy();
	}

	// 이미 캐릭터가 존재하는지 확인 
	if (ACharacter* ExistingCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Player character already exists! No need to respawn."));
		return; 
	}
	
	FVector SpawnLocation;
	if (LastCheckPoint)
	{
		FVector CheckpointLocation = LastCheckPoint->GetActorLocation();
		
		float RandomX = FMath::RandRange(-100.0f, 100.0f);
		float RandomY = FMath::RandRange(-100.0f, 100.0f);
		
		SpawnLocation = FVector(
			CheckpointLocation.X + RandomX,
			CheckpointLocation.Y + RandomY,
			CheckpointLocation.Z
		);
	}
	else
	{
		SpawnLocation = FVector(0, 0, 100);
	}
	
	FRotator SpawnRotator = FRotator(0, 0, 0);
	AMario64Character* NewCharacter = GetWorld()->SpawnActor<AMario64Character>(
		PlayerClassRef,
		SpawnLocation,
		SpawnRotator
	);
	
	if (NewCharacter)
	{
		
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			PC->Possess(NewCharacter);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn new character"));
	}
}
