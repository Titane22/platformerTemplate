// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlatformerTemplateGameMode.h"
#include "PlatformerTemplateCharacter.h"
#include "Public/Mario64Character.h"
#include "Character/PotatoCharacter.h"
#include "Character/FoxCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Public/Obstacles/CheckPoint_Flag.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

APlatformerTemplateGameMode::APlatformerTemplateGameMode()
{
	DefaultPawnClass = nullptr;
}

void APlatformerTemplateGameMode::BeginPlay()
{
	Super::BeginPlay();

	SpawnCharacter(FVector(0.0f, 0.0f, 100.0f));
}

void APlatformerTemplateGameMode::SpawnCharacter(FVector SpawnLocation)
{
	FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);

	Potato = GetWorld()->SpawnActor<APotatoCharacter>(
		PotatoCharacterClass,
		SpawnLocation + FVector(50.0f, 0.0f, 0.0f),
		SpawnRotation
	);

	Fox = GetWorld()->SpawnActor<AFoxCharacter>(
		FoxCharacterClass,
		SpawnLocation + FVector(-50.0f, 0.0f, 0.0f),
		SpawnRotation
	);

	Potato->SetPartner(Fox);
	Fox->SetPartner(Potato);

	if (Potato)
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			PC->Possess(Potato);

			if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
			{
				if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
				{
					// Potato 캐릭터에 DefaultMappingContext 추가
					if (Potato->GetDefaultMappingContext())
					{
						Subsystem->RemoveMappingContext(Potato->GetDefaultMappingContext());
						Subsystem->AddMappingContext(Potato->GetDefaultMappingContext(), 0);
					}
				}
			}
		}

		if (Fox)
		{
			Fox->SpawnDefaultController();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APlatformerTemplateGameMode::BeginPlay() Potato is null"));
	}
}

void APlatformerTemplateGameMode::RespawnPlayer()
{
	if (!Potato || !Fox)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerClassRef is Null in APlatformerTemplateGameMode::RespawnPlayer()"));
		return;
	}

	if (ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		PlayerCharacter->Destroy();
	}

	// TODO: 이미 캐릭터가 존재하는지 확인 
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
	
	SpawnCharacter(SpawnLocation);
}
