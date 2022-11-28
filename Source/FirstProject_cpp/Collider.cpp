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
	// Sphere�� ��Ʈ�� ����������
	SetRootComponent(SphereComponent_PSE);
	// �� �ݰ� ����
	SphereComponent_PSE->InitSphereRadius(40.0f);
	// Collision Preset�� Custom���� ����
	SphereComponent_PSE->SetCollisionProfileName(TEXT("Pawn_PSE"));

	MeshComponent_PSE2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent_PSE2->SetupAttachment(GetRootComponent());
	// �޽� ������Ʈ ���� ����� 
	// �� ����� Hard Coding�̶�� �ؼ� �� ���� ������ �� �� ������, �������Ʈ���� �׳� �ϴ°� �� ���� ���� ����
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComponentAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	if (MeshComponentAsset.Succeeded()) {
		// ���� �� ��δ�� ������ �ִٸ� 
		// �޽� ������Ʈ�� �� �������� �������� 
		MeshComponent_PSE2->SetStaticMesh(MeshComponentAsset.Object);
		// �̳��� �������Ʈ �ȿ��� ��ġ, ũ�� ���������� 
		MeshComponent_PSE2->SetRelativeLocation(FVector(0.f, 0.f, -40.f)); // == MeshComponentAsset�� ��ġ
		MeshComponent_PSE2->SetWorldScale3D(FVector(0.8f, 0.8f, 0.8f)); // == MeshComponentAsset�� ũ��
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
	OurMovementComponent->UpdatedComponent = RootComponent; // ���� ��Ʈ�� ���Ǿ�������Ʈ

	CameraInput = FVector2D(0.f, 0.f); // 2D�� X,Y 


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