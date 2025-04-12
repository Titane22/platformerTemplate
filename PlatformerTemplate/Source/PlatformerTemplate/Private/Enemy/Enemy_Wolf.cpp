// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy_Wolf.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

AEnemy_Wolf::AEnemy_Wolf()
	:Super()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->GravityScale = 3.0f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void AEnemy_Wolf::Die_Implementation()
{
	DieMontage = DieMont;
	Super::Die_Implementation();
}

void AEnemy_Wolf::OnDamaged_Implementation()
{
	
}

void AEnemy_Wolf::BeginPlay()
{
	Super::BeginPlay();
}
