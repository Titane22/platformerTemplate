// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../PlatformerTemplateCharacter.h"
#include "Components/BoxComponent.h"
#include "SideScrollerCharacter.generated.h"

// 레이캐스트 원점 구조체 정의
USTRUCT(BlueprintType)
struct FRaycastOrigins
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FVector2D TopLeft;

	UPROPERTY(BlueprintReadWrite)
	FVector2D TopRight;

	UPROPERTY(BlueprintReadWrite)
	FVector2D BottomLeft;

	UPROPERTY(BlueprintReadWrite)
	FVector2D BottomRight;

	FRaycastOrigins()
		: TopLeft(0, 0), TopRight(0, 0), BottomLeft(0, 0), BottomRight(0, 0)
	{
	}
};

UCLASS()
class PLATFORMERTEMPLATE_API ASideScrollerCharacter : public APlatformerTemplateCharacter
{
	GENERATED_BODY()

private:	
	/** 박스 콜리전 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoxCollision;

	FRaycastOrigins RaycastOrigins;

	float SkinWidth = 0.015f;

	// 가변 점프 관련 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MinJumpHeight = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MaxJumpHeight = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MaxJumpHoldTime = 0.6f;

	bool bJumpKeyHeld;
	float JumpHoldStartTime;

public:
	ASideScrollerCharacter();

protected:
	void UpdateRaycastOrigins();

	void CalculateRaySpacing();

	void VerticalCollisions(FVector Velocity);

	void HorizontalCollisions(FVector Velocity);

	virtual void Jump() override;

	virtual void StopJumping() override;

	int32 HorizontalRayCount = 4;

	int32 VerticalRyaCount = 4;

	float HorizontalRaySpacing;

	float VerticalRaySpacing;

	float HorizontalBoundLength;

	float VerticalBoundLength;

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;
public:
	/** Returns BoxCollision subobject **/
	FORCEINLINE class UBoxComponent* GetBoxCollision() const { return BoxCollision; }
};
