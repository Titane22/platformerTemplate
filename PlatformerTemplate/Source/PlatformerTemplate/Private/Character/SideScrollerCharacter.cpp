// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SideScrollerCharacter.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ASideScrollerCharacter::ASideScrollerCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = false; // 기존 이동 방향에 따른 회전을 비활성화
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 0.0f); // 회전 속도를 0으로 설정
	// 박스 콜리전 생성 및 설정
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(RootComponent);
	BoxCollision->SetBoxExtent(FVector(42.0f, 42.0f, 90.0f)); // 박스의 실제 크기로 설정해야 함
	BoxCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f)); // 캐릭터 중앙에 위치하도록 설정
	BoxCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); // 모든 채널에 대해 오버랩 설정

	 // 가변 점프 관련 변수 초기화
    MinJumpHeight = 300.0f;  // 최소 점프 높이
    MaxJumpHeight = 700.0f;  // 최대 점프 높이
    MaxJumpHoldTime = 0.6f;  // 최대 점프에 도달하는 키 홀드 시간
    bJumpKeyHeld = false;
    JumpHoldStartTime = 0.0f;
    
    // Tick 함수 활성화
    PrimaryActorTick.bCanEverTick = true;
}

void ASideScrollerCharacter::BeginPlay()
{
	Super::BeginPlay();
	FollowCamera->SetRelativeLocation(FVector(0.0f, 1000.0f, 150.0f));
    FollowCamera->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
}

void ASideScrollerCharacter::Tick(float DeltaTime)
{
	// 박스 콜리전 초기화 및 레이 원점 업데이트
	UpdateRaycastOrigins();

	// 점프 키가 눌려 있고 공중에 있는 경우
	if (bJumpKeyHeld && !GetCharacterMovement()->IsMovingOnGround())
	{
		// 키가 눌려진 시간 계산
		float HeldTime = GetWorld()->GetTimeSeconds() - JumpHoldStartTime;

		// 최대 홀드 시간을 초과하지 않았을 경우 추가 점프 높이 적용
		if (HeldTime <= MaxJumpHoldTime)
		{
			// 시간에 비례하여 추가 점프 높이 계산 (선형적 보간)
			float JumpBoost = FMath::Lerp(0.0f, MaxJumpHeight - MinJumpHeight, HeldTime / MaxJumpHoldTime);

			// Z 속도에 부스트 적용
			FVector Velocity = GetCharacterMovement()->Velocity;
			if (Velocity.Z > 0)
			{
				// 현재 높이가 아직 올라가는 중일 때만 추가 부스트 적용
				Velocity.Z += JumpBoost * DeltaTime * 4.0f;
				VerticalCollisions(Velocity);
				GetCharacterMovement()->Velocity = Velocity;
			}
		}
	}
	else
	{
		VerticalCollisions(GetCharacterMovement()->Velocity);
	}
}

void ASideScrollerCharacter::Jump()
{
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		GetCharacterMovement()->JumpZVelocity = MinJumpHeight;
		
		bJumpKeyHeld = true;
		JumpHoldStartTime = GetWorld()->GetTimeSeconds();
		
		Super::Jump();
	}
}

void ASideScrollerCharacter::StopJumping()
{
	bJumpKeyHeld = false;
	
	Super::StopJumping();
}

void ASideScrollerCharacter::UpdateRaycastOrigins()
{
	// 박스 콜리전의 경계 정보 가져오기
	FBoxSphereBounds BoxBounds = BoxCollision->CalcBounds(BoxCollision->GetComponentTransform());

	BoxBounds = BoxBounds.ExpandBy(SkinWidth * -2);

	RaycastOrigins.BottomLeft = FVector2D(BoxBounds.Origin.X - BoxBounds.BoxExtent.X, BoxBounds.Origin.Z - BoxBounds.BoxExtent.Z);
	RaycastOrigins.BottomRight = FVector2D(BoxBounds.Origin.X + BoxBounds.BoxExtent.X, BoxBounds.Origin.Z - BoxBounds.BoxExtent.Z);
	RaycastOrigins.TopLeft = FVector2D(BoxBounds.Origin.X - BoxBounds.BoxExtent.X, BoxBounds.Origin.Z + BoxBounds.BoxExtent.Z);
	RaycastOrigins.TopRight = FVector2D(BoxBounds.Origin.X + BoxBounds.BoxExtent.X, BoxBounds.Origin.Z + BoxBounds.BoxExtent.Z);

	DrawDebugBox(GetWorld(), BoxBounds.Origin, BoxBounds.BoxExtent, FQuat::Identity,
		FColor::Green, false, 0.0f, 0, 1.0f);
}

void ASideScrollerCharacter::CalculateRaySpacing()
{
	FBoxSphereBounds BoxBounds = BoxCollision->CalcBounds(BoxCollision->GetComponentTransform());
	BoxBounds = BoxBounds.ExpandBy(SkinWidth * -2);

	FVector Origin = BoxBounds.Origin;
	FVector BoxExtent = BoxBounds.BoxExtent * 2;

	HorizontalRayCount = FMath::Clamp(HorizontalRayCount, 2, TNumericLimits<int32>::Max());
	VerticalRyaCount = FMath::Clamp(VerticalRyaCount, 2, TNumericLimits<int32>::Max());

	HorizontalRaySpacing = BoxExtent.Z / (HorizontalRayCount - 1);
	VerticalRaySpacing = BoxExtent.X / (VerticalRyaCount - 1);

	HorizontalBoundLength = BoxExtent.X;
	VerticalBoundLength = BoxExtent.Z;
}

void ASideScrollerCharacter::VerticalCollisions(FVector Velocity)
{
	float DirectionZ = FMath::Sign(Velocity.Z);
	float RayLength = DirectionZ == 1 ? VerticalBoundLength : -VerticalBoundLength;

	for (int32 i = 0; i < VerticalRyaCount; i++)
	{
		FVector2D RaytOrigin = (DirectionZ == 1) ? RaycastOrigins.TopLeft : RaycastOrigins.BottomLeft;
		FVector Start = FVector(RaytOrigin.X + VerticalRaySpacing * i, 0.0f, RaytOrigin.Y);
		FVector End = Start + FVector(0.0f, 0.0f, RayLength);

		DrawDebugLine(
			GetWorld(),
			Start,
			End,
			FColor::Red,
			false,
			0.0f,
			0,
			1.0f
		);
	}
}

void ASideScrollerCharacter::HorizontalCollisions(FVector Velocity)
{
	float DirectionX = FMath::Sign(Velocity.X);
	float RayLength = DirectionX == -1 ? -HorizontalBoundLength : HorizontalBoundLength;
	FVector2D RayOrigin = DirectionX == -1 ? RaycastOrigins.BottomLeft : RaycastOrigins.BottomRight;

	for (int32 idx = 0; idx < HorizontalRayCount; idx++)
	{
		FVector Start = FVector(RayOrigin.X, 0.0f, RayOrigin.Y + HorizontalRaySpacing * idx);
		FVector End = Start + FVector(RayLength, 0.0f, 0.0f);

		DrawDebugLine(
			GetWorld(),
			Start,
			End,
			FColor::Red,
			false,
			0.0f,
			0,
			1.0f
		);
		/*FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(
			Hit,
			RayOrigin,

		)*/
	}
}
