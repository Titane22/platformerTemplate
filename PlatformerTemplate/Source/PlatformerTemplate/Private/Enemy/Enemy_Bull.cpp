// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy_Bull.h"
#include "Components/BoxComponent.h"

AEnemy_Bull::AEnemy_Bull()
	:Super()
{
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(RootComponent);


}
