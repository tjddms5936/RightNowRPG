// Fill out your copyright notice in the Description page of Project Settings.


#include "Floater.h"

// Sets default values
AFloater::AFloater()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh_PSE = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CustomStaticMesh"));
	// <>속에는 객체 유형을 지정
	// 그리고 함수 호출이기 때문에 우린 괄호+매개변수 필요
	Initialization_PSE = FVector(0.0f);
	PlacedLocation_PSE = FVector(0.0f); // 디폴트 해줌
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

	PlacedLocation_PSE = GetActorLocation(); // 순서가 중요함 

	if (bInitializeFloaterLocation_PSE) {
		SetActorLocation(Initialization_PSE);
	}

	BaseZLocation = PlacedLocation_PSE.Z;

	// SetActorLocation 설정 방법 3가지
	/* 방법 1.
	 SetActorLocation(FVector(0.0f, 0.0f, 0.0f));
	 BeginPlay 됐을 때 x,y,z값을 갖는 Floater 액터는 모두 0,0,0이 되도록 설정
	 이러면 Actor를 필드 아무대나 배치 해논뒤에 플레이하면 0,0,0의 위치에 Actor가 있음.
	 즉, Floater(C++클래스에 있는) Actor를 배치하면 겜 시작시 저절도 0,0,0
	 방법 2. Fvector 객체 생성후 초기화 한다음에 SecActorLocation 매개변수로 넣기
	 FVector Initialization = FVector(0.0f, 0.0f, 0.0f); // 초기화
	// == Fvector Initialization = FVector(0,0f) << 정의 되어있음 Fvector 구조체에
	 방법 3. 헤드파일에서 static mesh처럼 디테일 패널에 보이도록 설정 할 거임
	 PlacedLocation = GetActorLocation();  // 게임 시작 될 때 위치 저장하는 곳 초기화가 아니라 저장!!
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
		NewLocation.Z = BaseZLocation + A * FMath::Sin(B * RunningTime - C) + D;  // 초기값 RunningTime = 0 이니까 기존 Z + 1 이겠네
		// 결국 Sin함수는 -` ~ 1 사이를 Smooth하게 왓다갓다하니까 Z값은 위아래로 -1 ~ 1 만큼 왓다갓다하겠네
		SetActorLocation(NewLocation);
		RunningTime += DeltaTime; // 모든 테이크 아웃이 델타 시간만큼 증가

		/*FHitResult HitResult;
		AddActorLocalOffset(InitialDirection_PSE, false, &HitResult );
		FVector HitLocation = HitResult.Location;
		  로컬 위치값 지정 + true이면 엑터 물리법칙 적용*/

		  // UE_LOG(LogTemp, Warning, TEXT("Hit Location : X = %f, Y = %f, Z = %f"), HitLocation.X, HitLocation.Y, HitLocation.Z);   <-  출력 로그에서 상황 실시간으로 확인 가능
	}

	/*FRotator Rotation = FRotator(0.0f, 0.0f, 1.0f);
	AddActorLocalRotation(Rotation);
	매 틱마다 roll로 1도씩 회전함
	*/
}

