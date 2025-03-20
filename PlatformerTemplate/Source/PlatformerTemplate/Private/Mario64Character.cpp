// Fill out your copyright notice in the Description page of Project Settings.


#include "Mario64Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AMario64Character::AMario64Character()
{
	GetCharacterMovement()->MaxWalkSpeed = 230.f;
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 720.0f); 
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AMario64Character::OnHit);
}

void AMario64Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AMario64Character::Sprint);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMario64Character::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMario64Character::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMario64Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetCharacterMovement()->IsMovingOnGround() && 
		GetWorld()->GetTimeSeconds() - CurrentJumpTime > NextJumpMaxIdleTime)
	{
		JumpState = EJumpState::Single;
	}
}

void AMario64Character::Move(const FInputActionValue& Value)
{
	if (bIsJumping && bIsUTurn)
		return;
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		FVector NewDirection = ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X;
		UpdateMovementDirection(NewDirection);
		NewDirection.Z = 0.0f;
		
		float CurrentSpeed = GetCharacterMovement()->Velocity.Size2D(); // 수평 속도만 계산
		
		if (!NewDirection.IsNearlyZero())
		{
			// 현재 캐릭터 방향과 새 이동 방향 사이의 각도 계산
			float DotProduct = FVector::DotProduct(GetActorForwardVector(), NewDirection.GetSafeNormal());
			
			// U턴인 경우에만 회전 및 속도 조정 적용 (약 135도 이상 차이)
			if (DotProduct <= -0.7f)
			{
				float Angle = FMath::Atan2(NewDirection.Y, NewDirection.X);
				Angle = FMath::RoundToFloat(Angle / (PI / 4.0f)) * (PI / 4.0f);

				// 새 회전 적용
				FRotator NewRotator = FRotator(0.0f, FMath::RadiansToDegrees(Angle), 0.0f);
				SetActorRotation(NewRotator);
				
				// 속도 유지하며 방향 변경
				if (CurrentSpeed > 0.0f && GetCharacterMovement()->IsMovingOnGround())
				{
					FVector NewDirectionNormalized = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f);
					FVector NewVelocity = NewDirectionNormalized * CurrentSpeed;
					NewVelocity.Z = GetCharacterMovement()->Velocity.Z;
					
					GetCharacterMovement()->Velocity = NewVelocity;
				}
			}
		}
		
		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMario64Character::Look(const FInputActionValue& Value)
{
	Super::Look(Value);
}

void AMario64Character::Jump()
{
	if (GetCharacterMovement()->IsFalling() || bIsJumping)
	{
		if (bCanWallJump && GetWorld()->GetTimeSeconds() - CurrentWallHitTime <= WallJumpIdleTime)
		{
			PerformWallJump();
		}
		else
		{
			return;
		}
	}

	switch (JumpState)
	{
	case EJumpState::Single:
		GetCharacterMovement()->JumpZVelocity = 700.0f;
		GetCharacterMovement()->GravityScale = 2.0f;
		JumpState = EJumpState::Double;
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("Single"));
		break;
	case EJumpState::Double:
		GetCharacterMovement()->JumpZVelocity = 980.0f;
		GetCharacterMovement()->GravityScale = 2.2f;
		JumpState = EJumpState::Triple;
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Double"));
		break;
	case EJumpState::Triple:
		GetCharacterMovement()->JumpZVelocity = 1260.0f;
		GetCharacterMovement()->GravityScale = 2.4f;
		JumpState = EJumpState::Single;
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Triple"));
		break;
	}

	if (bIsUTurn && GetWorld()->GetTimeSeconds() - CurrentMoveTime <= UTurnIdleTime)
	{
		if (UTurnMont)
		{
			if (UAnimInstance* AnimInst = Cast<UAnimInstance>(GetMesh()->GetAnimInstance()))
			{
				AnimInst->Montage_Play(UTurnMont);
				FOnMontageEnded OnMontageEnded;
				OnMontageEnded.BindUObject(this, &AMario64Character::OnSideSomersaultMontageEnded);
				AnimInst->Montage_SetEndDelegate(OnMontageEnded);

				bIsJumping = true;
				return;
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("U-Turn Montage is Not Set"));
		}
	}
	bIsJumping = true;
	Super::Jump();
}

void AMario64Character::StopJumping()
{
	bIsJumping = false;
	Super::StopJumping();
}

void AMario64Character::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	CurrentJumpTime = GetWorld()->GetTimeSeconds();
	bCanWallJump = false;
	CurrentWallHitTime = 0.0f;
}

void AMario64Character::Sprint(const FInputActionValue& Value)
{
	bIsSprinting = Value.Get<bool>();

	if (bIsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 230.0f;
	}
}

void AMario64Character::UpdateMovementDirection(FVector NewDirection)
{
	PreviousDirection = CurrentDirection;

	if (!NewDirection.IsNearlyZero())
	{
		NewDirection.Normalize();
		CurrentDirection = NewDirection;
	}

	CheckForTurn();
}

void AMario64Character::CheckForTurn()
{
	if (!PreviousDirection.IsNearlyZero() && !CurrentDirection.IsNearlyZero())
	{
		float DotProduct = FVector::DotProduct(PreviousDirection, CurrentDirection);

		if (DotProduct <= -0.7f)
		{
			bIsUTurn = true;
			CurrentMoveTime = GetWorld()->GetTimeSeconds();
		}
		else if (bIsUTurn)
		{
			if (GetWorld()->GetTimeSeconds() - CurrentMoveTime > UTurnIdleTime)
			{
				bIsUTurn = false;
			}
		}

	}
}

void AMario64Character::Crouch(const FInputActionValue& Value)
{
	bool bIsCrouching = Value.Get<bool>();
	if (bIsCrouching)
	{
		Super::Crouch();
	}
	else
	{
		Super::UnCrouch();
	}
}

bool AMario64Character::IsWall(UPrimitiveComponent* Component, const FHitResult& Hit)
{
	// 충돌 응답 확인
	if (!Component || Component->GetCollisionResponseToChannel(ECC_Pawn) != ECR_Block)
	{
		return false;
	}

	// 충돌 법선 벡터 사용
	FVector SurfaceNormal = Hit.Normal;
	LastWallNormal = SurfaceNormal;
	CurrentWallHitTime = GetWorld()->GetTimeSeconds();

	return FMath::Abs(SurfaceNormal.Z) < 0.3f;
}

void AMario64Character::PerformWallJump()
{
	// 1. 벽 법선 벡터를 기본 방향으로 사용
	FVector JumpDirection = LastWallNormal;

	// 2. 수평 성분만 정규화 (X, Y만)
	FVector HorizontalDir = FVector(JumpDirection.X, JumpDirection.Y, 0.0f);
	if (!HorizontalDir.IsNearlyZero())
	{
		HorizontalDir.Normalize();
	}

	// 3. 수직 방향 추가 (약 45도 위쪽으로)
	FVector FinalJumpDir = HorizontalDir + FVector(0, 0, 1.0f);
	FinalJumpDir.Normalize();

	// 4. 벽 점프 힘 적용
	float WallJumpForce = 1700.0f; 
	LaunchCharacter(FinalJumpDir * WallJumpForce, true, true);

	// 5. 방향 설정 (선택적)
	// 점프 방향으로 캐릭터 회전
	SetActorRotation(FinalJumpDir.Rotation());
}

void AMario64Character::OnSideSomersaultMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsJumping = false;
	bIsUTurn = false;

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("OnSideSomersaultMontageEnded"));
}

void AMario64Character::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Somthing Hit"));

	//if (OtherActor && OtherActor != this)
	{
		if (IsWall(OtherComp, Hit) && bIsJumping)
		{
			OnWallHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
		}
		else
		{
			OnObjectHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
		}
	}
}

void AMario64Character::OnWallHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("OnWallHit!!!!!!!!!!"));
	bCanWallJump = true;
}

void AMario64Character::OnObjectHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("OnObjectHit!!!!!!!!!!"));
}

