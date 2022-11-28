// Fill out your copyright notice in the Description page of Project Settings.


#include "Collider.h"
#include "Components/Spherecomponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "FirstProject_cpp/ColliderPawnMovementComponent.h"

// Sets default values
ACollider::ACollider()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent_PSE = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	// Sphere을 루트로 만들어버리기
	SetRootComponent(SphereComponent_PSE);
	// 구 반경 설정
	SphereComponent_PSE->InitSphereRadius(40.0f);
	// Collision Preset을 Custom으로 설정
	SphereComponent_PSE->SetCollisionProfileName(TEXT("Pawn_PSE"));

	MeshComponent_PSE2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent_PSE2->SetupAttachment(GetRootComponent());
	// 메쉬 컴포넌트 에셋 만들기 
	// 이 방법은 Hard Coding이라고 해서 더 빠른 개발을 할 순 있지만, 블루프린트에서 그냥 하는게 더 나을 때도 있음
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComponentAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	if (MeshComponentAsset.Succeeded()) {
		// 만약 저 경로대로 에셋이 있다면 
		// 메쉬 컴포넌트를 그 에셋으로 설정하자 
		MeshComponent_PSE2->SetStaticMesh(MeshComponentAsset.Object);
		// 이놈의 블루프린트 안에서 위치, 크기 설정해주자 
		MeshComponent_PSE2->SetRelativeLocation(FVector(0.f, 0.f, -40.f)); // == MeshComponentAsset의 위치
		MeshComponent_PSE2->SetWorldScale3D(FVector(0.8f, 0.8f, 0.8f)); // == MeshComponentAsset의 크기
	}

	SpringarmComponent_PSE = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringarmComponent_PSE->SetupAttachment(GetRootComponent());
	SpringarmComponent_PSE->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	SpringarmComponent_PSE->TargetArmLength = 400.f;
	SpringarmComponent_PSE->bEnableCameraLag = true;
	SpringarmComponent_PSE->CameraLagSpeed = 3.0f;

	CameraComponent_PSE = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent_PSE->SetupAttachment(SpringarmComponent_PSE, USpringArmComponent::SocketName);

	OurMovementComponent = CreateDefaultSubobject<UColliderPawnMovementComponent>(TEXT("OurPawnMovementComponent"));
	OurMovementComponent->UpdatedComponent = RootComponent; // 현재 루트는 스피어컴포넌트

	CameraInput = FVector2D(0.f, 0.f); // 2D는 X,Y 


	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ACollider::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACollider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += CameraInput.X;
	SetActorRotation(NewRotation);

	FRotator NewSpringArmRotation = SpringarmComponent_PSE->GetComponentRotation();
	NewSpringArmRotation.Pitch = FMath::Clamp(NewSpringArmRotation.Pitch += CameraInput.Y, -80.f, -15.f);
	SpringarmComponent_PSE->SetWorldRotation(NewSpringArmRotation);
}

// Called to bind functionality to input
void ACollider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACollider::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACollider::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("CameraPitch"), this, &ACollider::PitchCamera);
	PlayerInputComponent->BindAxis(TEXT("CameraYaw"), this, &ACollider::YawCamera);
}

void ACollider::MoveForward(float input_value) {
	FVector Forward = GetActorForwardVector();
	if (OurMovementComponent)
	{
		OurMovementComponent->AddInputVector(Forward * input_value);
	}
}

void ACollider::MoveRight(float input_value) {
	FVector Right = GetActorRightVector();
	if (OurMovementComponent)
	{
		OurMovementComponent->AddInputVector(Right * input_value);
	}
}

void ACollider::YawCamera(float AxisValue) {
	CameraInput.X = AxisValue;
}

void ACollider::PitchCamera(float AxisValue) {
	CameraInput.Y = AxisValue;
}


UPawnMovementComponent* ACollider::GetMovementComponent() const {
	return OurMovementComponent;
}