// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/LakituCamera.h"
#include "Mario64Character.h"
#include "GameFramework/Character.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
ULakituCamera::ULakituCamera()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	//
	TargetArmLength = DefaultDistance;
	bWasColliding = false;
	CollisionAdjustmentFactor = 0.0f;
	CurrentInterpolationSpeed = CameraSmoothSpeed;
	CurrentCameraOffset = FVector::ZeroVector;
	TargetCameraOffset = FVector::ZeroVector;
}


// Called when the game starts
void ULakituCamera::BeginPlay()
{
	Super::BeginPlay();

	// ...
	InitializeCamera();

	if (AMario64Character* OwnerCharacter = Cast<AMario64Character>(GetOwner()))
	{
		PreviousCharacterLocation = OwnerCharacter->GetActorLocation();
		CharacterRef = OwnerCharacter;
	}
	bIsCharacterGrounded = true;

}


// Called every frame
void ULakituCamera::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (!Camera || !CameraBoom)
		return;

	CheckCharacterGrounded();

	UpdateCameraXYPosition();
}

void ULakituCamera::InitializeCamera()
{
	if(AActor* Owner = GetOwner())
	{
		Camera = Owner->FindComponentByClass<UCameraComponent>();
		CameraBoom = Owner->FindComponentByClass<USpringArmComponent>();
		if(!CameraBoom)
		{
			CameraBoom = NewObject<USpringArmComponent>(Owner, TEXT("Camera Boom"));
			CameraBoom->SetupAttachment(Owner->GetRootComponent());
			CameraBoom->RegisterComponent();

			CameraBoom->bUsePawnControlRotation = true;
			CameraBoom->TargetArmLength = DefaultDistance;
			CameraBoom->SocketOffset = FVector(0.0f, 0.0f, CameraHeight);
			CameraBoom->bEnableCameraLag = true;
			CameraBoom->bDoCollisionTest = false;
		}
		if (!Camera)
		{
			Camera = NewObject<UCameraComponent>(Owner, TEXT("Lakitu Camera"));
			Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
			Camera->FieldOfView = 50.0f;
			Camera->RegisterComponent();

			Camera->bUsePawnControlRotation = true;
		}
	}
}

void ULakituCamera::SmoothCameraPosition(float DeltaTime)
{ 
	if (!Camera || !CameraBoom)
		return;

	CameraBoom->TargetArmLength = FMath::FInterpTo(
		CameraBoom->TargetArmLength,
		TargetArmLength,
		DeltaTime,
		CurrentInterpolationSpeed
	);

	Camera->SetRelativeRotation(FMath::RInterpTo(
		Camera->GetRelativeRotation(),
		TargetCameraRotation,
		DeltaTime,
		CurrentInterpolationSpeed
	));
}

void ULakituCamera::HandleCameraCollision()
{
	if (!CameraBoom)
		return;

	FHitResult Hit;
	FVector StartLocation = GetOwner()->GetActorLocation();
	FVector EndLocation = StartLocation + (CameraBoom->GetComponentRotation().Vector() * -TargetArmLength);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	if (GetWorld()->LineTraceSingleByChannel(
		Hit,
		StartLocation,
		EndLocation,
		ECC_Camera,
		QueryParams
	))
	{
		float NewTargetArmLength = (Hit.Location - StartLocation).Size() * 0.9f;// 충돌 후 거리 조정
		if (!bWasColliding)
		{
			CurrentInterpolationSpeed = CollisionSmoothSpeed;
			bWasColliding = true;
		}

		CollisionAdjustmentFactor = FMath::Clamp(NewTargetArmLength / TargetArmLength, 0.0f, 1.0f);

		CameraBoom->TargetArmLength = FMath::FInterpTo(
			CameraBoom->TargetArmLength,
			NewTargetArmLength,
			GetWorld()->GetDeltaSeconds(),
			CurrentInterpolationSpeed
		);

		DrawDebugSphere(GetWorld(), Hit.Location, 10.0f, 8, FColor::Red, false, -1.0f, 0, 1.0f);
	}
	else if(bWasColliding)
	{
		CurrentInterpolationSpeed = CameraSmoothSpeed;
		bWasColliding = false;
		CollisionAdjustmentFactor = 1.0f;
	}
}

void ULakituCamera::ResetCamera()
{
	if (!CameraBoom || !GetOwner())
		return;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn || !OwnerPawn->Controller)
		return;

	FRotator TargetRotation = OwnerPawn->GetActorRotation();
	TargetRotation.Pitch = TargetCameraRotation.Pitch;

	OwnerPawn->Controller->SetControlRotation(TargetRotation);

	CurrentInterpolationSpeed = CameraSmoothSpeed * 2.0f;
}

void ULakituCamera::RotateCameraAroundTarget(float Yaw, float Pitch)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn || !OwnerPawn->Controller)
		return;

	FRotator CurrentRotation = OwnerPawn->GetControlRotation();
	
	float AdjustedPitch = Pitch * PitchSensitivity * -1.0f;
	float NewPitch = FMath::Clamp(CurrentRotation.Pitch + AdjustedPitch, -80.0f, 80.0f);
	float NewYaw = CurrentRotation.Yaw + Yaw;

	OwnerPawn->Controller->SetControlRotation(FRotator(NewPitch, NewYaw, 0.0f));
}

void ULakituCamera::AdjustCameraForMovement(float DeltaTime)
{
	if (!GetOwner())
		return;

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character || !CameraBoom)
		return;

	// Get character's velocity
	FVector Velocity = Character->GetVelocity();
	float Speed = Velocity.Size2D(); // Only consider horizontal movement
	
	// Get camera's forward direction in world space (ignoring pitch)
	FRotator CameraRotation = CameraBoom->GetComponentRotation();
	CameraRotation.Pitch = 0.0f;
	FVector CameraForward = CameraRotation.Vector();
	FVector CameraRight = FRotator(0.0f, CameraRotation.Yaw + 90.0f, 0.0f).Vector();

	// Reset target offset if character is not moving
	if (Speed < MovementThreshold)
	{
		TargetCameraOffset = FVector::ZeroVector;
	}
	else
	{
		// Project velocity onto camera right direction to determine if moving left or right
		float RightMovement = FVector::DotProduct(Velocity.GetSafeNormal2D(), CameraRight);
		
		// Calculate offset based on movement direction
		// Moving left (negative RightMovement) -> positive offset (camera moves right)
		// Moving right (positive RightMovement) -> negative offset (camera moves left)
		float OffsetMagnitude = FMath::Clamp(FMath::Abs(RightMovement) / 300.0f, 0.0f, 1.0f) * MaxCameraOffset;
		float OffsetDirection = -FMath::Sign(RightMovement);
		
		TargetCameraOffset = CameraRight * OffsetDirection * OffsetMagnitude;
	}
	
	// Smoothly interpolate to the target offset
	CurrentCameraOffset = FMath::VInterpTo(
		CurrentCameraOffset,
		TargetCameraOffset,
		DeltaTime,
		CameraOffsetSmoothing
	);
	
	// Apply the offset to the spring arm socket offset
	FVector CurrentSocketOffset = CameraBoom->SocketOffset;
	CurrentSocketOffset.X = CurrentCameraOffset.X;
	CurrentSocketOffset.Y = CurrentCameraOffset.Y;
	CurrentSocketOffset.Z = CameraHeight; // Maintain the height
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("CurrentSocketOffset: %s"), *CurrentSocketOffset.ToString()));
	CameraBoom->SocketOffset = CurrentSocketOffset;
}

void ULakituCamera::CheckCharacterGrounded()
{
	if (!CharacterRef)
		return;

	if (UCharacterMovementComponent* OwnerMovementComp = CharacterRef->GetCharacterMovement())
	{
		bIsCharacterGrounded = OwnerMovementComp->IsMovingOnGround();
	}
	else
	{
		FVector StartLocation = CharacterRef->GetActorLocation();
		FVector EndLocation = StartLocation - FVector(0.0f, 0.0f, 20.0f);

		FHitResult Hit;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(CharacterRef);

		bIsCharacterGrounded = GetWorld()->LineTraceSingleByChannel(
			Hit,
			StartLocation,
			EndLocation,
			ECC_Visibility,
			QueryParams
		);
	}
}

void ULakituCamera::UpdateCameraXYPosition()
{
	if (!CameraBoom || !GetOwner())
		return;
	
	FVector CharacterLocation = GetOwner()->GetActorLocation();
	
	AActor* CameraBoomOwner = CameraBoom->GetOwner();
	if (CameraBoomOwner == GetOwner())
	{
		// 현재 SpringArm의 상대 위치 가져오기
		FVector RelativeLocation = CameraBoom->GetRelativeLocation();
		
		// 기존 XY 유지
		FVector NewRelativeLocation = RelativeLocation;
		
		// Z축만 조정
		static float DefaultZHeight = RelativeLocation.Z; // 처음 Z 높이 저장

		// 카메라 시야 내에 캐릭터가 있는지 확인
		bool bIsCharacterInCameraView = IsCharacterInCameraView();
		
		// Z축 조정 부분
		float ZDiff = PreviousCharacterLocation.Z - CharacterLocation.Z;
		/*GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red,
			FString::Printf(TEXT("Z Adjust: %f"), ZDiff));*/
		
		// 다음 조건에서 카메라 Z 위치 재설정:
		// 1. 캐릭터가 땅에 있을 때 
		// 2. 큰 낙하가 있을 때 (7.0 이상)
		// 3. 캐릭터가 카메라 시야를 벗어났을 때
		if (bIsCharacterGrounded || ZDiff > 7.0f || !bIsCharacterInCameraView)
		{
			// 카메라 높이 재설정
			float TargetZ = DefaultZHeight;
			
			// 카메라 시야를 벗어난 경우, 부드럽게 캐릭터 위치로 이동
			if (!bIsCharacterInCameraView)
			{
				// 캐릭터가 카메라보다 위에 있는지 아래에 있는지 확인
				float CameraWorldZ = CameraBoomOwner->GetActorLocation().Z + RelativeLocation.Z;
				float CharacterWorldZ = CharacterLocation.Z;
				
				// 캐릭터 방향으로 부드럽게 카메라 조정
				TargetZ = DefaultZHeight + (CharacterWorldZ - CameraWorldZ) * 0.5f;
				
				/*GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow,
					TEXT("Character out of view - Adjusting camera"));*/
			}
			
			// 부드러운 전환을 위해 보간 사용
			NewRelativeLocation.Z = FMath::FInterpTo(
				RelativeLocation.Z,
				TargetZ,
				GetWorld()->GetDeltaSeconds(),
				5.0f 
			);
		}
		else
		{
			// 일반적인 공중 상태에서는 차이값 적용
			NewRelativeLocation.Z += ZDiff;
		}
		
		CameraBoom->SetRelativeLocation(NewRelativeLocation);
	}
	
	PreviousCharacterLocation = CharacterLocation;
}

// 캐릭터가 카메라 시야 내에 있는지 확인하는 함수
bool ULakituCamera::IsCharacterInCameraView()
{
	if (!Camera || !GetOwner())
		return true; // 정보가 없으면 기본적으로 시야 내에 있다고 가정
	
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
		return true;
	
	// 캐릭터 위치를 스크린 좌표로 변환
	FVector CharacterLocation = GetOwner()->GetActorLocation();
	FVector2D ScreenLocation;
	bool bIsOnScreen = PC->ProjectWorldLocationToScreen(CharacterLocation, ScreenLocation);
	
	// 화면 크기 가져오기
	int32 ViewportSizeX, ViewportSizeY;
	PC->GetViewportSize(ViewportSizeX, ViewportSizeY);
	
	// 마진 설정 (화면 가장자리 10% 내에 들어오면 시야 밖으로 간주)
	float MarginX = ViewportSizeX * 0.1f;
	float MarginY = ViewportSizeY * 0.1f;
	
	// 화면 내에 있고, 마진 내에 없는지 확인
	bool bIsWithinView = bIsOnScreen && 
						ScreenLocation.X > MarginX && 
						ScreenLocation.X < (ViewportSizeX - MarginX) &&
						ScreenLocation.Y > MarginY && 
						ScreenLocation.Y < (ViewportSizeY - MarginY);
	
	return bIsWithinView;
}
