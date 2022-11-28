// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"


// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	StartPoint = FVector(0.f);
	EndPoint = FVector(0.f);

	bInterping = false;

	InterpSpeed = 4.f;
	InterpTime = 1.f; // 1초 
}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	StartPoint = GetActorLocation();
	// 현재 EndPoint는 로컬 기준임 why?  meta = (MakeEditWidget = "true") 때문. 
	// 보간을 위해선 월드 위치가 필요 따라서 
	EndPoint += StartPoint;

	/*
	void SetTimer(
	FTimerHandle & InOutHandle, UserClass * InObj, 
	typename FTimerDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr InTimerMethod,
	float InRate)*/
	// SetTimer : 지정된 기본 기능을 설정된 간격으로 호출하도록 타이머를 설정합니다. 이 핸들에 대해 타이머가 이미 설정되어 있으면 현재 타이머를 대체합니다.
	GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpTime);
	// 게임 시작하면 bInterping은 true인 상태
	Distance = (EndPoint - StartPoint).Size(); // -> float
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterping) {
		FVector CurrentLocation = GetActorLocation();
		// Floating 하기 위한 기법. 보간법
		FVector Interp = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, InterpSpeed);
		SetActorLocation(Interp);

		float DistanceTraveled = (GetActorLocation() - StartPoint).Size();
		if (DistanceTraveled == Distance) {
			ToggleInterping();
			// 만약 목표지점까지 도달했다면, bInterping을 false로 바꿔줌

			GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpTime);
			SwapVectors(StartPoint, EndPoint);
		}
	}
}


void AFloatingPlatform::ToggleInterping()
{
	// bInterping이 true상태라면 false로 바꿔주고 
	// bInterpingdl false상태라면 ture로 바꿔주는
	// bool swap이라고 생각하면 됨.
	bInterping = !bInterping;
}

void AFloatingPlatform::SwapVectors(FVector& Vec1, FVector& Vec2)
{
	FVector tmp;
	tmp = Vec1;
	Vec1 = Vec2;
	Vec2 = tmp;
}