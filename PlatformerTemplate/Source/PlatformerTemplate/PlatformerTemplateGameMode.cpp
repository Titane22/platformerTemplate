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
#include "GameFramework/PlayerStart.h"

APlatformerTemplateGameMode::APlatformerTemplateGameMode()
{
	DefaultPawnClass = nullptr;
}

void APlatformerTemplateGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("Level started: %s"), *GetWorld()->GetName());

	AActor* PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass());
	if (PlayerStart)
	{
		SpawnCharacter(PlayerStart->GetActorLocation());
	}
	else
	{
		SpawnCharacter(FVector(0.0f, 0.0f, 100.0f));
	}
}

void APlatformerTemplateGameMode::SpawnCharacter(FVector SpawnLocation)
{
	FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);

	SpawnLocation.Z += 100.0f; 

	Potato = GetWorld()->SpawnActor<APotatoCharacter>(
		PotatoCharacterClass,
		SpawnLocation + FVector(50.0f, 0.0f, 0.0f),
		SpawnRotation
	);

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
			SpawnPartner();
		}

	}
	else
	{
		if (!Potato) UE_LOG(LogTemp, Error, TEXT("Potato character failed to spawn"));
		if (!PotatoCharacterClass) UE_LOG(LogTemp, Error, TEXT("PotatoCharacterClass is not set!"));
	}
}

void APlatformerTemplateGameMode::SpawnPartner()
{
	FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);

	Fox = GetWorld()->SpawnActor<AFoxCharacter>(
		FoxCharacterClass,
		Potato->GetActorLocation() + FVector(-100.0f, 0.0f, 0.0f),
		SpawnRotation
	);

	if (Fox)
	{
		Fox->SetPartner(Potato);
		Potato->SetPartner(Fox);
		Fox->SpawnDefaultController();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Fox character failed to spawn"));
		if (!FoxCharacterClass) UE_LOG(LogTemp, Error, TEXT("FoxCharacterClass is not set!"));
	}
}

void APlatformerTemplateGameMode::RespawnPlayer()
{
	if (!Potato || !Fox)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerClassRef is Null in APlatformerTemplateGameMode::RespawnPlayer()"));
		return;
	}

	Potato->Destroy();
	Fox->Destroy();

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

void APlatformerTemplateGameMode::SetCheckpoint(ACheckPoint_Flag* ToSetFlag, AMario64Character* IndicatorCharacterRef, bool bIsPortal)
{
	LastCheckPoint = ToSetFlag;
	if (!Fox || !Potato)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fox or Potato is NULL"));
		return;
	}
	if (!IndicatorCharacterRef)
		return;

	bool bIsPotatoPlayer = false;

	if (APotatoCharacter* tempPotato = Cast<APotatoCharacter>(IndicatorCharacterRef))
	{
		bIsPotatoPlayer = true;
	}

	float Distance = FVector::Distance(Potato->GetActorLocation(), Fox->GetActorLocation());

	if (Distance >= 100.0f)
	{
		FVector SpawnLocation = ToSetFlag->GetActorLocation() + FVector(0.0f, 150.0f, 50.0f);
		FRotator SpawnRotration = FRotator(0.0f, 0.0f, 0.0f);

		if (bIsPotatoPlayer)
		{
			TeleportCharacter(Fox, SpawnLocation, SpawnRotration);
		}
		else
		{
			TeleportCharacter(Potato, SpawnLocation, SpawnRotration);
		}
		if (bIsPortal)
		{
			TeleportCharacter(IndicatorCharacterRef, ToSetFlag->GetActorLocation() + FVector(0.0f, -150.0f, 50.0f), SpawnRotration);
		}
	}
}

void APlatformerTemplateGameMode::TeleportCharacter(AMario64Character* ToTeleportCharacter, const FVector& SpawnLocation, const FRotator& SpawnRotration)
{
	ToTeleportCharacter->SetActorLocationAndRotation(SpawnLocation, SpawnRotration);

	if (TeleportEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			TeleportEffect,
			SpawnLocation,
			SpawnRotration
		);
	}

	if (TeleportSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			TeleportSound,
			SpawnLocation
		);
	}
}
