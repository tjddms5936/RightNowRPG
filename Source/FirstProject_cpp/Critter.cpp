// Fill out your copyright notice in the Description page of Project Settings.


#include "Critter.h"
#include "Camera/CameraComponent.h"
// Sets default values
ACritter::ACritter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	MeshComponent_PSE = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	MeshComponent_PSE->SetupAttachment(GetRootComponent());

	Camera_PSE = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera_PSE->SetupAttachment(GetRootComponent());
	Camera_PSE->SetRelativeLocation(FVector(-300.f, 0.f, 300.f));
	Camera_PSE->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));

	// AutoPossessPlayer = EAutoReceiveInput::Player0;
	CurrentVelocity = FVector(0.f);
	MaxSpeed = 100.f;
}

// Called when the game starts or when spawned
void ACritter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACritter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
	// �ð� �� �Ÿ��� ����ϱ� ���� Deltatime�� ������� �Ѵ�. 
	SetActorLocation(NewLocation);
}

// Called to bind functionality to input
void ACritter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACritter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACritter::MoveRight);
}

void ACritter::MoveForward(float value) {
	CurrentVelocity.X = FMath::Clamp(value, -1.f, 1.f) * MaxSpeed; // value ���� -1.f ~ 1.f �� ���̷θ� �� ����
}

void ACritter::MoveRight(float value) {
	CurrentVelocity.Y = FMath::Clamp(value, -1.f, 1.f) * MaxSpeed;
}

