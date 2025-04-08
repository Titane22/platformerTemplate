// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FoxCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AFoxCharacter::AFoxCharacter()
	:Super()
{
}

void AFoxCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
