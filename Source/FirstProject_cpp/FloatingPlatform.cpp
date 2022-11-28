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
	InterpTime = 1.f; // 1�� 
}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	StartPoint = GetActorLocation();
	// ���� EndPoint�� ���� ������ why?  meta = (MakeEditWidget = "true") ����. 
	// ������ ���ؼ� ���� ��ġ�� �ʿ� ���� 
	EndPoint += StartPoint;

	/*
	void SetTimer(
	FTimerHandle & InOutHandle, UserClass * InObj, 
	typename FTimerDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr InTimerMethod,
	float InRate)*/
	// SetTimer : ������ �⺻ ����� ������ �������� ȣ���ϵ��� Ÿ�̸Ӹ� �����մϴ�. �� �ڵ鿡 ���� Ÿ�̸Ӱ� �̹� �����Ǿ� ������ ���� Ÿ�̸Ӹ� ��ü�մϴ�.
	GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpTime);
	// ���� �����ϸ� bInterping�� true�� ����
	Distance = (EndPoint - StartPoint).Size(); // -> float
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterping) {
		FVector CurrentLocation = GetActorLocation();
		// Floating �ϱ� ���� ���. ������
		FVector Interp = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, InterpSpeed);
		SetActorLocation(Interp);

		float DistanceTraveled = (GetActorLocation() - StartPoint).Size();
		if (DistanceTraveled == Distance) {
			ToggleInterping();
			// ���� ��ǥ�������� �����ߴٸ�, bInterping�� false�� �ٲ���

			GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpTime);
			SwapVectors(StartPoint, EndPoint);
		}
	}
}


void AFloatingPlatform::ToggleInterping()
{
	// bInterping�� true���¶�� false�� �ٲ��ְ� 
	// bInterpingdl false���¶�� ture�� �ٲ��ִ�
	// bool swap�̶�� �����ϸ� ��.
	bInterping = !bInterping;
}

void AFloatingPlatform::SwapVectors(FVector& Vec1, FVector& Vec2)
{
	FVector tmp;
	tmp = Vec1;
	Vec1 = Vec2;
	Vec2 = tmp;
}