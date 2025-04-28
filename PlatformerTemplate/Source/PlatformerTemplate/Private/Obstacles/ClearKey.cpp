// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacles/ClearKey.h"
#include "Mario64Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AClearKey::AClearKey()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	KeyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Key Mesh"));
    KeyMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    KeyMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    KeyMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	KeyMesh->SetupAttachment(RootComponent);

	CheckVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CheckVolume"));
    CheckVolume->OnComponentBeginOverlap.AddDynamic(this, &AClearKey::OnOverlapBegin);
    CheckVolume->SetupAttachment(KeyMesh);
}

// Called when the game starts or when spawned
void AClearKey::BeginPlay()
{
	Super::BeginPlay();
	
	// 초기 트랜스폼 저장
	OriginalTransform = GetActorTransform();
}

void AClearKey::ResetKey()
{
    PlayerRef = nullptr;
	bIsOwned = false;
	
    SetActorEnableCollision(true);
    
    if (!OriginalTransform.Equals(FTransform::Identity))
    {
        SetActorTransform(OriginalTransform);
    }
}

void AClearKey::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMario64Character* Player = Cast<AMario64Character>(OtherActor))
	{
		Player->SetClearKey(this);
		PlayerRef = Player;
		bIsOwned = true;
		
		CheckVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		if (OriginalTransform.Equals(FTransform::Identity))
		{
		    OriginalTransform = GetActorTransform();
		}

        if (GettingKeySound)
        {
            UGameplayStatics::PlaySoundAtLocation(GetWorld(), GettingKeySound, GetActorLocation());
        }
	}
}

// Called every frame
void AClearKey::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bIsOwned || !PlayerRef)
	{
	    return;
	}
	
	// 플레이어가 유효한지 추가 확인 (죽었을 때 처리)
    if (!IsValid(PlayerRef))
    {
        ResetKey();
        return;
    }
    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("AClearKey::Tick"));
    // 키 움직임 처리
    try
    {
        FVector PlayerLocation = PlayerRef->GetActorLocation();
        
        float CurrentTime = GetWorld()->GetTimeSeconds();
        float Angle = CurrentTime * 2.0f; // 회전 속도 조절
        
        // 원형 궤도 반경 설정
        float OrbitRadius = 120.0f;
        
        // 상하 움직임을 위한 사인 함수 (부드러운 상하운동)
        float VerticalOffset = FMath::Sin(CurrentTime * 3.0f) * 30.0f;
        
        // 회전 궤도 계산 (X, Y축은 플레이어 주변을 회전, Z축은 상하운동)
        FVector OrbitPosition = FVector(
            FMath::Cos(Angle) * OrbitRadius,
            FMath::Sin(Angle) * OrbitRadius,
            100.0f + VerticalOffset // 플레이어 기준 높이 + 상하 움직임
        );

        //FVector TargetLocation = PlayerLocation + OrbitPosition;

        //FVector DirectionToPlayer = PlayerLocation - GetActorLocation();
        //FRotator TargetRotation = DirectionToPlayer.Rotation();

        //FVector NewLocation = FMath::Lerp(GetActorLocation(), TargetLocation, DeltaTime * 5.0f);

        //SetActorLocation(NewLocation);
        //SetActorRotation(FMath::Lerp(GetActorRotation(), TargetRotation, DeltaTime * 5.0f));

        //KeyMesh->AddLocalRotation(FRotator(0.0f, 5.0f, -7.0f) * DeltaTime * 5.0f);

        FVector TargetLocation = PlayerLocation + OrbitPosition;
        FVector NewLocation = FMath::Lerp(GetActorLocation(), TargetLocation, DeltaTime * 50.0f);
        SetActorLocation(FMath::Lerp(GetActorLocation(), NewLocation, DeltaTime * 5.0f));
        
        FRotator CurrentRotation = GetActorRotation();
        FRotator NewRotation = CurrentRotation + FRotator(0.0f, 30.0f * DeltaTime, 0.0f);
        SetActorRotation(NewRotation);
        
        KeyMesh->AddLocalRotation(FRotator(0.0f, 0.0f, 20.0f * DeltaTime));
    }
    catch(...)
    {
        ResetKey();
    }
}

