// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySkillBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"
#include "Main.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
// Sets default values
AEnemySkillBase::AEnemySkillBase()
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
	bFromEnemy = false;
}

// Called when the game starts or when spawned
void AEnemySkillBase::BeginPlay()
{
	Super::BeginPlay();
	/* 이 밑에 부분은  파생 스킬 클래스에서 각각 설정해주자 */

	// 몬스터에게 맞지 않아도 3초뒤면 저절로 없어지도록 설정.
	GetWorldTimerManager().SetTimer(DestroyTime, this, &AEnemySkillBase::Disappear, DestroyDelay);

	// HitCollision 콜리전 설정. Enemy빼곤 다 Ignore. 
	HitCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	HitCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	HitCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	HitCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemySkillBase::HitOnOverlapBegin);
	HitCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemySkillBase::HitOnOverlapEnd);
	/*if (SkillOnSound) {
		UGameplayStatics::PlaySound2D(this, SkillOnSound);
	}*/
}

// Called every frame
void AEnemySkillBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySkillBase::HitOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/* 이 밑에 부분은  파생 스킬 클래스에서 각각 설정해주자 */
	if (OtherActor) {
		if (bFromEnemy) return;
		else {
			AMain* Main = Cast<AMain>(OtherActor);
			if (Main) {
				UE_LOG(LogTemp, Warning, TEXT("Skill & Main Overlap!"));
				// 스킬 이펙트가 적이랑 오버랩된다면! 적에게 데미지 입혀야 함.
				SkillDamage = SkillDamageRand();
				if (DamageTypeClass) {
					// 적에게 데미지 입히는 작업. 적의 TakeDamage함수가 발동 되는 것임.
					UGameplayStatics::ApplyDamage(Main, SkillDamage, SkillInstigator, this, DamageTypeClass);
					UE_LOG(LogTemp, Warning, TEXT("Main take damage!"));
				}

				// 맞는 이펙트도 있으면 좋을 것 같은데 흠. 
				// 1. 우선 오버랩 됐을 때 스킬이펙트의 위치를 구하자. 
				// 2. 스폰이미터를 해서. 따로 맞았을 경우의 이펙트를 적용시켜주자 
				if (HitParticle) {
					FVector HittedSkillLocation = GetActorLocation();
					// 일단 이 스킬의 HitParticle이 설정 되어 있어야 함! 
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HittedSkillLocation, FRotator(0.f), true); // true의미 : 이펙트 자동 Destroy
					UE_LOG(LogTemp, Warning, TEXT("Enemy Skill Destroy"));
					
				}
				Main->BeHittedStart();
				/*if (HitSuccessSound) {
					UGameplayStatics::PlaySound2D(this, HitSuccessSound);
				}*/
				Disappear(); // 스킬 이펙트 없애고
			}
		}
	}
}

void  AEnemySkillBase::HitOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor) {
		// 버프인경우 
		if (bFromEnemy) {
			AEnemy* Enemy = Cast<AEnemy>(OtherActor);
			if (Enemy) {
				// FAttachmentTransformRules Rule = FAttachmentTransformRules::KeepRelativeTransform;
				// AttachToActor(Main, Rule, Main->GetMesh()->GetSocketBoneName("BuffSkillLocation"));
				const USkeletalMeshSocket* BuffSkillLocation = Enemy->GetMesh()->GetSocketByName("BuffSkillLocation");
				if (BuffSkillLocation) {
					BuffSkillLocation->AttachActor(this, Enemy->GetMesh());
				}
				UE_LOG(LogTemp, Warning, TEXT("Attach buff to Enemy"));
			}
		}
		else {
			AMain* Main = Cast<AMain>(OtherActor);
			if (Main) {
				UE_LOG(LogTemp, Warning, TEXT("Skill & Main Overlap!"));
				// 스킬 이펙트가 적이랑 오버랩된다면! 적에게 데미지 입혀야 함.
				SkillDamage = SkillDamageRand();
				if (DamageTypeClass) {
					// 적에게 데미지 입히는 작업. 적의 TakeDamage함수가 발동 되는 것임.
					UGameplayStatics::ApplyDamage(Main, SkillDamage, SkillInstigator, this, DamageTypeClass);
					UE_LOG(LogTemp, Warning, TEXT("Main take damage!"));
				}

				// 맞는 이펙트도 있으면 좋을 것 같은데 흠. 
				// 1. 우선 오버랩 됐을 때 스킬이펙트의 위치를 구하자. 
				// 2. 스폰이미터를 해서. 따로 맞았을 경우의 이펙트를 적용시켜주자 
				if (HitParticle) {
					FVector HittedSkillLocation = GetActorLocation();
					// 일단 이 스킬의 HitParticle이 설정 되어 있어야 함! 
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HittedSkillLocation, FRotator(0.f), true); // true의미 : 이펙트 자동 Destroy
					UE_LOG(LogTemp, Warning, TEXT("Enemy Skill Destroy"));
				}
				Main->BeHittedStart();
				/*if (HitSuccessSound) {
					UGameplayStatics::PlaySound2D(this, HitSuccessSound);
				}*/
				// Disappear(); // 적 스킬이 내 캐릭에서 스폰되면 없애지 않음
			}
		}
	}
}

void AEnemySkillBase::Disappear() {
	Destroy();
}

float AEnemySkillBase::SkillDamageRand()
{
	return FMath::RandRange(this->MinDmg, this->MaxDmg);
}