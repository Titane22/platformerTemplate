// Fill out your copyright notice in the Description page of Project Settings.


#include "Mario64Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Enemy/PT_Enemy.h"
#include "Obstacles/ClearKey.h"
#include "Obstacles/Ladder.h"
#include "Component/LakituCamera.h"
#include "Kismet/GameplayStatics.h"
#include "../PlatformerTemplateGameMode.h"

AMario64Character::AMario64Character()
{
	LakituCameraComponent = CreateDefaultSubobject<ULakituCamera>(TEXT("Lakitu"));

	LegPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Leg Point"));
	LegPoint->SetupAttachment(RootComponent);
	JumpMaxHoldTime = 0.3f;

	GetCharacterMovement()->MaxWalkSpeed = 230.f;
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->RotationRate = FRotator(720.0f, 0.0f, 0.0f);
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
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AMario64Character::Crouch);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMario64Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 점프 상태 업데이트
	if (GetCharacterMovement()->IsMovingOnGround() && 
		GetWorld()->GetTimeSeconds() - CurrentJumpTime > NextJumpMaxIdleTime)
	{
		JumpState = EJumpState::Single;
	}

	if (CurrentState == EActionState::Climbing)
	{
		// 올라가는 속도가 있는지 확인
		float VerticalSpeed = GetCharacterMovement()->Velocity.Z;
		if (FMath::Abs(VerticalSpeed) > 1.0f)
		{
			// 입력이 없는데 속도가 있다면 강제로 속도 리셋
			if (!bClimbingHasInput)
			{
				GetCharacterMovement()->Velocity = FVector::ZeroVector;
			}
		}
		
		bClimbingHasInput = false;
	}

	if (CurrentState == EActionState::Climbing && bCameDownLadder)
	{
		FVector StartLocation = LegPoint->GetComponentLocation();
		FVector EndLocation = StartLocation + GetActorUpVector() * -60.0f;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		FHitResult Hit;
		DrawDebugLine(
			GetWorld(),
			StartLocation,
			EndLocation,
			FColor::Green
		);

		if (GetWorld()->LineTraceSingleByChannel(
			Hit,
			StartLocation,
			EndLocation,
			ECC_Visibility,
			QueryParams
		))
		{
			ToggleClimbing(false, nullptr);
		}
	}

	// 라키투 카메라 기능 활용
	if (LakituCameraComponent)
	{
		LakituCameraComponent->AdjustCameraForMovement(DeltaSeconds);
	}
}

void AMario64Character::Move(const FInputActionValue& Value)
{
	if (CurrentState == EActionState::Burrowed || bIsUTurn)
		return;
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	if (Controller != nullptr)
	{
		if (CurrentState == EActionState::Climbing)
		{
			// 입력 임계값 증가 (0.2로 설정)
			if (FMath::Abs(MovementVector.Y) < 0.2f)
			{
				// 이동 입력이 없으면 속도 0으로 설정하고 즉시 반환
				GetCharacterMovement()->Velocity = FVector::ZeroVector;
				return;
			}
			bCameDownLadder = MovementVector.Y < 0.0f;
			bClimbingHasInput = true;

			FVector CurrentVelocity = GetCharacterMovement()->Velocity;
			float TargetZSpeed = MovementVector.Y * 100.0f; 
			
			float NewZSpeed = FMath::FInterpTo(CurrentVelocity.Z, TargetZSpeed, GetWorld()->GetDeltaSeconds(), 5.0f);
			
			GetCharacterMovement()->Velocity = FVector(0.0f, 0.0f, NewZSpeed);

			FVector StartLocation = LegPoint->GetComponentLocation();
			FVector EndLocation = StartLocation + GetActorForwardVector() * 60.0f;
			
			FHitResult Hit;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);
			
			DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, false, 0.1f);
			
			// 전방에 사다리가 있는지 확인
			if (!GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_Visibility, QueryParams))
			{
				// 대각선 방향 계산 (전방 + 상방, 75도 각도)
				// 75도 = 전방 벡터 1.0에 대해 상방 벡터 약 3.73 (tan 75° ≈ 3.73)
				FVector UpVector = FVector::UpVector;
				FVector DiagonalDirection = GetActorForwardVector() + (UpVector * 3.73f);
				DiagonalDirection.Normalize();
				
				FVector LaunchVelocity = DiagonalDirection * 850.0f; // 속도 조절
				
				LaunchCharacter(LaunchVelocity, true, true);
				
				DrawDebugLine(GetWorld(), StartLocation, StartLocation + LaunchVelocity * 0.1f, 
					FColor::Red, false, 1.0f, 0, 3.0f);
			}
		}
		else
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
}

void AMario64Character::Look(const FInputActionValue& Value)
{
	FVector2D RotationValue = Value.Get<FVector2D>();
	if (LakituCameraComponent && !RotationValue.IsZero())
	{
		LakituCameraComponent->RotateCameraAroundTarget(RotationValue.X, RotationValue.Y);
	}
}

void AMario64Character::Jump()
{
	// TODO: Check if Max Speed
	if (CurrentState == EActionState::Crouching && bWasRunning)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("CurrentState == EActionState::Crouching && bWasRunning"));
		if (GetWorld()->GetTimeSeconds() - CurrentCrouchTime <= LongJumpIdelTime)
		{
			return PerformLongJump();
		}
	}

	// 공중에 있거나 이미 점프 중인 경우
	if (GetCharacterMovement()->IsFalling() || CurrentState == EActionState::Flying)
	{
		// 벽 점프가 가능하고 허용 시간 내에 있는 경우
		if (GetWorld()->GetTimeSeconds() - CurrentWallHitTime <= WallJumpIdleTime)
		{
			PerformWallJump();
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, TEXT("벽 점프 실행!"));
			return;
		}
		else
		{
			// 벽 점프 조건이 아니면 추가 점프 안함
			return;
		}
	}

	// 지상에서의 점프 로직 (단일, 이중, 삼중 점프)
	switch (JumpState)
	{
	case EJumpState::Single:
		GetCharacterMovement()->JumpZVelocity = 400.0f;
		GetCharacterMovement()->GravityScale = IdleGravityScale;
		JumpState = EJumpState::Double;
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("Single"));
		break;
	case EJumpState::Double:
		GetCharacterMovement()->JumpZVelocity = 600.0f;
		GetCharacterMovement()->GravityScale = DoubleJumpGravityScale;
		JumpState = EJumpState::Triple;
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Double"));
		break;
	case EJumpState::Triple:
		GetCharacterMovement()->JumpZVelocity = 780.0f;
		GetCharacterMovement()->GravityScale = TripleJumpGravityScale;
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

				CurrentState = EActionState::Flying;
				return;
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("U-Turn Montage is Not Set"));
		}
	}
	bIsJumping = true;
	CurrentState = EActionState::Flying;
	Super::Jump();
}

void AMario64Character::PerformLongJump()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Jump::bIsCrouching && bWasRunning"));
	Super::UnCrouch();

	GetCharacterMovement()->JumpZVelocity = 700.0f;
	GetCharacterMovement()->GravityScale = TripleJumpGravityScale;

	FVector ForwardDirection = GetActorForwardVector() * 1800.0f;
	ForwardDirection.Z = 1250.0f;

	LaunchCharacter(ForwardDirection, true, true);

	SetState(EActionState::Flying);
	bWasRunning = false;
	return;
}

void AMario64Character::StopJumping()
{
	bIsJumping = false;
	Super::StopJumping();
}

void AMario64Character::Landed(const FHitResult& Hit)
{
	if (APT_Enemy* Enemy = Cast<APT_Enemy>(Hit.GetActor()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Destroy"));
		LaunchCharacter(FVector(0.0f, 0.0f, 1000.0f), true, true);
		Enemy->Destroy();
	}
	else
	{
		Super::Landed(Hit);

		CurrentJumpTime = GetWorld()->GetTimeSeconds();
		CurrentState = EActionState::Idle;
		bCanWallJump = false;
		CurrentWallHitTime = 0.0f;
		if (bIsSprinting)
		{
			GetCharacterMovement()->MaxWalkSpeed = 500.0f;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = 230.0f;
		}
	}
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

	if(bUseU_Turn)
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
	bool bWantToCrouching = Value.Get<bool>();
	if (bWantToCrouching)
	{
		bWasRunning = bIsSprinting;
		SetState(EActionState::Crouching);

		if (bWasRunning)
		{
			//SetState(EActionState::Sliding);
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Crouch::bWasRunning"));
			// TODO: 슬라이딩 커브
		}
		else
		{
			Super::Crouch();
		}
	}
	else
	{
		CurrentState = EActionState::Idle;
		bWasRunning = false;
		GetCharacterMovement()->MaxWalkSpeed = 230.f;
		Super::UnCrouch();
	}
}

bool AMario64Character::IsWall(UPrimitiveComponent* Component, const FHitResult& Hit)
{
	if (!Component)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("IsWall: 충돌 컴포넌트 없음"));
		return false;
	}
	
	// 충돌 응답 확인
	if (Component->GetCollisionResponseToChannel(ECC_Pawn) != ECR_Block)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("IsWall: 충돌 응답 Block 아님"));
		return false;
	}

	// 충돌 법선 벡터 사용
	FVector SurfaceNormal = Hit.Normal;
	
	// 벽으로 인식되는 경우 (수직 표면)
	bool bIsVerticalSurface = FMath::Abs(SurfaceNormal.Z) < 0.3f;
	
	if (bIsVerticalSurface)
	{
		LastWallNormal = SurfaceNormal;
		CurrentWallHitTime = GetWorld()->GetTimeSeconds();
	}

	return bIsVerticalSurface;
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

	// 3. 수직 방향 추가 
	FVector FinalJumpDir = HorizontalDir + FVector(0, 0, 1.0f);
	FinalJumpDir.Normalize();

	// 4. 벽 점프 힘 적용
	float WallJumpForce = 1700.0f; 
	LaunchCharacter(FinalJumpDir * WallJumpForce, true, true);
	CurrentState = EActionState::Flying;

	// 5. 점프 방향으로 캐릭터 회전
	SetActorRotation(FinalJumpDir.Rotation());
}

void AMario64Character::ResetLevel()
{
	// TODO: Check Current Death Count
	UWorld* CurrentWorld = GetWorld();
	if (!CurrentWorld)
	{
		UE_LOG(LogTemp, Error, TEXT("ResetLevel: World is null!"));
		return;
	}
	
	// 게임모드 안전하게 가져오기
	if (APlatformerTemplateGameMode* GameMode = Cast<APlatformerTemplateGameMode>(UGameplayStatics::GetGameMode(CurrentWorld)))
	{
		// 체크포인트가 있으면 RespawnPlayer 사용
		if (GameMode->LastCheckPoint)
		{
			GameMode->RespawnPlayer();
		}
		else
		{
			// TODO: Crash
			FString CurrentLevelName = CurrentWorld->GetName();
			if (!CurrentLevelName.IsEmpty())
			{
				if (APlayerController* PC = Cast<APlayerController>(GetController()))
				{
					DisableInput(PC);
					PC->UnPossess();
				}
				
				UGameplayStatics::OpenLevel(CurrentWorld, FName(*CurrentLevelName), false);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("AMario64Character::ResetLevel: Cannot get level name!"));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AMario64Character::ResetLevel: GameMode is null!"));
	}
}

void AMario64Character::SetState(const EActionState NewState)
{
	if (CurrentState == NewState)
		return;

	EActionState PrevState = CurrentState;
	CurrentState = NewState;

	switch (CurrentState)
	{
	case EActionState::Sliding:
		GetCharacterMovement()->MaxWalkSpeed = 350.0f;
		break;
	case EActionState::Flying:
		break;
	case EActionState::Crouching:
		CurrentCrouchTime = GetWorld()->GetTimeSeconds();
		GetCharacterMovement()->MaxWalkSpeed = 150.0f;
		break;
	case EActionState::Climbing:
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->GravityScale = DisableGravityScale;
		GetCharacterMovement()->MaxWalkSpeed = 50.0f;
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
		GetCharacterMovement()->StopMovementImmediately();
		break;
	case EActionState::Die:
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->GravityScale = DisableGravityScale;
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			DisableInput(PC);
		}
		break;
	case EActionState::Burrowed:
		break;
	case EActionState::Idle:
	default:
		GetCharacterMovement()->GravityScale = IdleGravityScale;
		GetCharacterMovement()->MaxWalkSpeed = 230.f;
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("bIsMovementDisabled is false"));
		break;
	}
}

void AMario64Character::OnSideSomersaultMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsUTurn = false;
	CurrentState = EActionState::Idle;
}

float AMario64Character::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsInvulnerable || CurrentState == EActionState::Die)
		return 0.0f;

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	CurrentHealth -= ActualDamage;

	bIsInvulnerable = true;
	GetWorldTimerManager().SetTimer(
		InvulnerabilityTimerHandle,
		this,
		&AMario64Character::EndInvulnerability,
		InvulnerabilityTime,
		false
	);

	OnDamaged();

	if (CurrentHealth <= 0.0f)
	{
		Die();
	}

	return ActualDamage;
}

void AMario64Character::Die_Implementation()
{
	if (ClearKeyRef)
	{
		ClearKeyRef->ResetKey();
		ClearKeyRef = nullptr;
	}

	SetState(EActionState::Die);

	FTimerHandle DieTimerHandle;
	GetWorldTimerManager().SetTimer(
		DieTimerHandle,
		this,
		&AMario64Character::ResetLevel,
		2.0f,
		false
	);
}

void AMario64Character::OnDamaged_Implementation()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->AddImpulse(FVector(0, 0, 400.0f), true);
	}
}

void AMario64Character::EndInvulnerability()
{
	bIsInvulnerable = false;
}

void AMario64Character::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (IsWall(OtherComp, Hit))
	{
		OnWallHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
	}
	else
	{
		OnObjectHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
	}
}

void AMario64Character::OnWallHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	bCanWallJump = true;
}

void AMario64Character::OnObjectHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("OnObjectHit!!!!!!!!!!"));
}

void AMario64Character::ToggleClimbing(bool ToSetState, ALadder* ToSetLadder)
{
	bIsClimbing = ToSetState;
	CurrentLadderRef = ToSetLadder;
	if (ToSetState)
	{
		if (CurrentLadderRef)
		{
			// 사다리의 Forward 방향 가져오기
			FVector ForwardVector = CurrentLadderRef->GetActorForwardVector();
			FRotator BaseRotation = ForwardVector.Rotation();
			
			// 캐릭터가 사다리를 정면으로 바라보도록 90도 회전
			FRotator NewRotation = FRotator(0.0f, BaseRotation.Yaw - 90.0f, 0.0f);
			
			// 회전 적용
			SetActorRotation(NewRotation);
		}
		
		SetState(EActionState::Climbing);
	}
	else
	{
		SetState(EActionState::Idle);
		bCameDownLadder = false;
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

void AMario64Character::SetClearKey(AClearKey* ToSetKey)
{
	ClearKeyRef = ToSetKey;
}

