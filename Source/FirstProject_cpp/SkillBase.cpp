// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"
#include "Main.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"
// Sets default values
ASkillBase::ASkillBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = Scene;

	SkillParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SkillParticle"));
	SkillParticle->SetupAttachment(GetRootComponent());


	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 1200.f;
	ProjectileMovement->MaxSpeed = 1200.f;
	ProjectileMovement->ProjectileGravityScale = 0.f; // 0이상이면 가다가 떨어짐 

	HitCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("HitCollision"));
	HitCollision->SetupAttachment(GetRootComponent());

	
	DestroyDelay = 3.f;

	SkillDamage = 10.f;
	MinDmg = 10.f;
	MaxDmg = 10.f;

	// 버프같은 캐릭터 따라가면 true로 지정해주자
	bFromChar = false;
}

// Called when the game starts or when spawned
void ASkillBase::BeginPlay()
{
	Super::BeginPlay();
	/* 이 밑에 부분은  파생 스킬 클래스에서 각각 설정해주자 */

	// 몬스터에게 맞지 않아도 3초뒤면 저절로 없어지도록 설정.
	GetWorldTimerManager().SetTimer(DestroyTime, this, &ASkillBase::Disappear, DestroyDelay);

	// HitCollision 콜리전 설정. Enemy빼곤 다 Ignore. 
	HitCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	HitCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	HitCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	HitCollision->OnComponentBeginOverlap.AddDynamic(this, &ASkillBase::HitOnOverlapBegin);
	HitCollision->OnComponentEndOverlap.AddDynamic(this, &ASkillBase::HitOnOverlapEnd);

	SkillCase = CheckCase();
	if (SkillStartSound) {
		UGameplayStatics::PlaySound2D(this, SkillStartSound);
	}
}

// Called every frame
void ASkillBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASkillBase::HitOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) {
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		switch (SkillCase)
		{
		case 1: // 버프스킬이라 Begin인식이 안됨.
			break;
		case 2: // 원거리 광역스킬(메테오) Destroy도 여기서 타이머로 설정
			if (Enemy) { // 스킬에 닿은놈이 적이기만 하면 됨
				SkillDamage = SkillDamageRand();
				if (DamageTypeClass) {
					UGameplayStatics::ApplyDamage(Enemy, SkillDamage, SkillInstigator, this, DamageTypeClass);
				}
				if (HitParticle) {
					FVector HittedSkillLocation = GetActorLocation();
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HittedSkillLocation);
				}
				if (SkillHittedSound) {
					UGameplayStatics::PlaySound2D(this, SkillHittedSound);
				}
				float RandHitted = FMath::RandRange(1, 3);
				if (RandHitted == 1) {
					// 1/3 확률로 적은 맞는 모션 취함. 
					Enemy->BeHittedStart(); // BeHittedEnd() 함수는 애니메이션BP에서 노티파이 이벤트 시작되면 호출되도록 설정
				}
				// 타이머로 스킬Actor 파괴
				GetWorldTimerManager().SetTimer(DestroyTime, this, &ASkillBase::Disappear, DestroyDelay);
			}
			break;

		case 3: // 원거리 단일스킬(에너지볼트) Destroy도 여기서 설정
			if (Enemy && Enemy->MainReference) { // 에너지볼트에 닿았는데 적이 있고, Main 캐릭터도 이 적의 원거리 범위 안에 들어와 있다면
				if (Enemy->MainReference->CombatTarget == Enemy) { // 또 그 때 Main 캐릭터의 타게팅된 적이 이 에너지볼트가 닿은 적이랑 같다면 
					SkillDamage = SkillDamageRand();
					if (DamageTypeClass) {
						UGameplayStatics::ApplyDamage(Enemy, SkillDamage, SkillInstigator, this, DamageTypeClass);
					}
					if (HitParticle) {
						FVector HittedSkillLocation = GetActorLocation(); // 적이 맞는 순간의 그 위치
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HittedSkillLocation);
					}
					if (SkillHittedSound) {
						UGameplayStatics::PlaySound2D(this, SkillHittedSound);
					}
					float RandHitted = FMath::RandRange(1, 3);
					if (RandHitted == 1) {
						// 1/3 확률로 적은 맞는 모션 취함. 
						Enemy->BeHittedStart(); // BeHittedEnd() 함수는 애니메이션BP에서 노티파이 이벤트 시작되면 호출되도록 설정
					}
					// 닿자마자 스킬은 파괴됨
					Disappear();
				}
			}
			break;

		case 4:
			// 4,5번은 적주변 생성 스킬이라 Begin인식이 안됨. ^^ 바로 break
			if (Enemy) { // 광역스킬이라 일단 맞은게 적이기만 하면 됨
				SkillDamage = SkillDamageRand();
				if (DamageTypeClass) {
					UGameplayStatics::ApplyRadialDamage(GetWorld(), SkillDamage, Enemy->MainReference->CombatTarget->GetActorLocation(), 1, DamageTypeClass, TArray<AActor*>(), this);
					UE_LOG(LogTemp, Warning, TEXT("Box size is %f"), HitCollision->GetScaledBoxExtent().Size() / 100);
				}
				if (HitParticle) {
					FVector HittedSkillLocation = GetActorLocation();
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HittedSkillLocation);
				}
				if (SkillHittedSound) {
					UGameplayStatics::PlaySound2D(this, SkillHittedSound);
				}
				float RandHitted = FMath::RandRange(1, 3);
				if (RandHitted == 1) {
					// 1/3 확률로 적은 맞는 모션 취함. 
					Enemy->BeHittedStart(); // BeHittedEnd() 함수는 애니메이션BP에서 노티파이 이벤트 시작되면 호출되도록 설정
				}
				// 타이머로 스킬Actor 파괴
				GetWorldTimerManager().SetTimer(DestroyTime, this, &ASkillBase::Disappear, DestroyDelay);
			}
			break;
		case 5:
			break;
		default:
			break;
		}
	}
}

void  ASkillBase::HitOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor) {
		AMain* Main = Cast<AMain>(OtherActor);
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		switch (SkillCase)
		{
		case 1: // 버프스킬 인식해야됨
			if (Main) {
				CustomBuffSkill();
			}
			break;
		case 2: // 원거리 광역스킬(메테오) End 인식 필요 x
			break;

		case 3: // 원거리 단일스킬(에너지볼트) End 인식 필요 x
			break;

		case 4:
			// 4,5번은 적주변 생성 스킬이라 스폰되면 멈춰있어보이지만 실제로는 움직임. End될때 시간 딜레이 줘서 Destroy주면 됨
			//if (Enemy) { // 광역스킬이라 일단 맞은게 적이기만 하면 됨
			//	SkillDamage = SkillDamageRand();
			//	if (DamageTypeClass) {
			//		UGameplayStatics::ApplyDamage(Enemy, SkillDamage, SkillInstigator, this, DamageTypeClass);
			//	}
			//	if (HitParticle) {
			//		FVector HittedSkillLocation = GetActorLocation();
			//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HittedSkillLocation);
			//	}
			//	if (SkillHittedSound) {
			//		UGameplayStatics::PlaySound2D(this, SkillHittedSound);
			//	}
			//	float RandHitted = FMath::RandRange(1, 3);
			//	if (RandHitted == 1) {
			//		// 1/3 확률로 적은 맞는 모션 취함. 
			//		Enemy->BeHittedStart(); // BeHittedEnd() 함수는 애니메이션BP에서 노티파이 이벤트 시작되면 호출되도록 설정
			//	}
			//	// 타이머로 스킬Actor 파괴
			//	GetWorldTimerManager().SetTimer(DestroyTime, this, &ASkillBase::Disappear, DestroyDelay);
			//}
			break;
		case 5:
			if (Enemy && Enemy->MainReference) {
				if (Enemy->MainReference->CombatTarget == Enemy) {
					SkillDamage = SkillDamageRand();
					if (DamageTypeClass) {
						UGameplayStatics::ApplyDamage(Enemy, SkillDamage, SkillInstigator, this, DamageTypeClass);
					}
					if (HitParticle) {
						FVector HittedSkillLocation = GetActorLocation();
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HittedSkillLocation);
					}
					if (SkillHittedSound) {
						UGameplayStatics::PlaySound2D(this, SkillHittedSound);
					}
					float RandHitted = FMath::RandRange(1, 3);
					if (RandHitted == 1) {
						// 1/3 확률로 적은 맞는 모션 취함. 
						Enemy->BeHittedStart(); // BeHittedEnd() 함수는 애니메이션BP에서 노티파이 이벤트 시작되면 호출되도록 설정
					}
					// 타이머로 스킬Actor 파괴
					GetWorldTimerManager().SetTimer(DestroyTime, this, &ASkillBase::Disappear, DestroyDelay);
				}
			}
			break;
		default:
			break;
		}
	}
}

int32 ASkillBase::CheckCase() {
	if (bFromChar) {
		if (bIsBuff) return 1; // 버프스킬 
		else {
			if (bIsWideDmg) return 2; // 원거리 광역스킬(메테오)
			else return 3; // 원거리 단일 스킬(에너지 볼트)
		}
	}
	else {
		if (bIsWideDmg) return 4; // 적주변 생성 광역스킬(블랙홀)
		else return 5; // 적주변 생성 단일스킬(가시?)
	}
}





void ASkillBase::Disappear() {
	/*
	블루프린트에서 호출 가능하며, 스킬 Destroy는 스킬 각각마다 타이밍 적절하게 해주자.
	광역 스킬 : 적 주변에서 생성(c++구현) && 일정시간 흐른뒤 Destroy(블루프린트 구현) && Enemy || Boss 면 다 데미지 입힘(c++구현)
	단일 스킬 + 적주변 : 적 주변에서 생성(c++구현) && 일정시간 흐른뒤 Destroy(블루프린트 구현) && CombatTarget만 데미지 입힘(c++구현)
	단일 스킬 + 캐릭터로부터 : 캐릭터에서부터 생성(c++구현) && 닿으면 Destroy(블루프린트 구현) && Enemy || Boss 면 데미지 입힘
	Buff 스킬 : 캐릭터에서부터 생성(c++구현) && 닿으면 없애는게 아니라 Attach해줌 캐릭터에(c++구현) && 그냥 전부 다 c++ 구현할까? ㅇㅇ..;;;

	4가지 조건에 따라서 설정만 해주면 되니까 !!!!
	*/ 
	
	Destroy();
}

float ASkillBase::SkillDamageRand()
{
	return FMath::RandRange(this->MinDmg, this->MaxDmg);
}

