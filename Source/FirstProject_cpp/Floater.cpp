// Fill out your copyright notice in the Description page of Project Settings.


#include "Floater.h"

// Sets default values
AFloater::AFloater()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh_PSE = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CustomStaticMesh"));
	// <>�ӿ��� ��ü ������ ����
	// �׸��� �Լ� ȣ���̱� ������ �츰 ��ȣ+�Ű����� �ʿ�
	Initialization_PSE = FVector(0.0f);
	PlacedLocation_PSE = FVector(0.0f); // ����Ʈ ����
	WorldOrigin_PSE = FVector(0.0f, 0.0f, 0.0f);
	bInitializeFloaterLocation_PSE = false;
	InitialDirection_PSE = FVector(0.0f, 0.0f, 0.0f);
	bShoultFloat_PSE = false;
	InitialForce = FVector(2000000.f, 0.0f, 0.0f);
	InitialTorque = FVector(2000000.f, 0.0f, 0.0f);

	RunningTime = 0.f;

	A = 0.f;
	B = 0.f;
	C = 0.f;
	D = 0.f;
}

// Called when the game starts or when spawned
void AFloater::BeginPlay()
{
	Super::BeginPlay();
	float InitialX = FMath::FRandRange(-500.f, 500.f);
	float InitialY = FMath::FRandRange(-500.f, 0.f);
	float InitialZ = FMath::FRandRange(0.f, 500.f);

	Initialization_PSE.X = InitialX;
	Initialization_PSE.Y = InitialY;
	Initialization_PSE.Z = InitialZ;

	PlacedLocation_PSE = GetActorLocation(); // ������ �߿��� 

	if (bInitializeFloaterLocation_PSE) {
		SetActorLocation(Initialization_PSE);
	}

	BaseZLocation = PlacedLocation_PSE.Z;

	// SetActorLocation ���� ��� 3����
	/* ��� 1.
	 SetActorLocation(FVector(0.0f, 0.0f, 0.0f));
	 BeginPlay ���� �� x,y,z���� ���� Floater ���ʹ� ��� 0,0,0�� �ǵ��� ����
	 �̷��� Actor�� �ʵ� �ƹ��볪 ��ġ �س�ڿ� �÷����ϸ� 0,0,0�� ��ġ�� Actor�� ����.
	 ��, Floater(C++Ŭ������ �ִ�) Actor�� ��ġ�ϸ� �� ���۽� ������ 0,0,0
	 ��� 2. Fvector ��ü ������ �ʱ�ȭ �Ѵ����� SecActorLocation �Ű������� �ֱ�
	 FVector Initialization = FVector(0.0f, 0.0f, 0.0f); // �ʱ�ȭ
	// == Fvector Initialization = FVector(0,0f) << ���� �Ǿ����� Fvector ����ü��
	 ��� 3. ������Ͽ��� static meshó�� ������ �гο� ���̵��� ���� �� ����
	 PlacedLocation = GetActorLocation();  // ���� ���� �� �� ��ġ �����ϴ� �� �ʱ�ȭ�� �ƴ϶� ����!!
	 StaticMesh_PSE->AddForce(InitialForce);
	 StaticMesh_PSE->AddTorque(InitialTorque);
	 AddActorLocalOffset(Localoffset, false, &HitResult);
	 AddActorWorldOffset(Localoffset, false, &HitResult);
	 */
}

// Called every frame
void AFloater::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bShoultFloat_PSE) {

		FVector NewLocation = GetActorLocation();
		NewLocation.Z = BaseZLocation + A * FMath::Sin(B * RunningTime - C) + D;  // �ʱⰪ RunningTime = 0 �̴ϱ� ���� Z + 1 �̰ڳ�
		// �ᱹ Sin�Լ��� -` ~ 1 ���̸� Smooth�ϰ� �Ӵٰ����ϴϱ� Z���� ���Ʒ��� -1 ~ 1 ��ŭ �Ӵٰ����ϰڳ�
		SetActorLocation(NewLocation);
		RunningTime += DeltaTime; // ��� ����ũ �ƿ��� ��Ÿ �ð���ŭ ����

		/*FHitResult HitResult;
		AddActorLocalOffset(InitialDirection_PSE, false, &HitResult );
		FVector HitLocation = HitResult.Location;
		  ���� ��ġ�� ���� + true�̸� ���� ������Ģ ����*/

		  // UE_LOG(LogTemp, Warning, TEXT("Hit Location : X = %f, Y = %f, Z = %f"), HitLocation.X, HitLocation.Y, HitLocation.Z);   <-  ��� �α׿��� ��Ȳ �ǽð����� Ȯ�� ����
	}

	/*FRotator Rotation = FRotator(0.0f, 0.0f, 1.0f);
	AddActorLocalRotation(Rotation);
	�� ƽ���� roll�� 1���� ȸ����
	*/
}

