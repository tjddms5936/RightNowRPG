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
	ProjectileMovement->ProjectileGravityScale = 0.f; // 0�̻��̸� ���ٰ� ������ 

	HitCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("HitCollision"));
	HitCollision->SetupAttachment(GetRootComponent());


	DestroyDelay = 3.f;

	SkillDamage = 10.f;
	MinDmg = 10.f;
	MaxDmg = 10.f;

	// �������� ĳ���� ���󰡸� true�� ����������
	bFromEnemy = false;
}

// Called when the game starts or when spawned
void AEnemySkillBase::BeginPlay()
{
	Super::BeginPlay();
	/* �� �ؿ� �κ���  �Ļ� ��ų Ŭ�������� ���� ���������� */

	// ���Ϳ��� ���� �ʾƵ� 3�ʵڸ� ������ ���������� ����.
	GetWorldTimerManager().SetTimer(DestroyTime, this, &AEnemySkillBase::Disappear, DestroyDelay);

	// HitCollision �ݸ��� ����. Enemy���� �� Ignore. 
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
	/* �� �ؿ� �κ���  �Ļ� ��ų Ŭ�������� ���� ���������� */
	if (OtherActor) {
		if (bFromEnemy) return;
		else {
			AMain* Main = Cast<AMain>(OtherActor);
			if (Main) {
				UE_LOG(LogTemp, Warning, TEXT("Skill & Main Overlap!"));
				// ��ų ����Ʈ�� ���̶� �������ȴٸ�! ������ ������ ������ ��.
				SkillDamage = SkillDamageRand();
				if (DamageTypeClass) {
					// ������ ������ ������ �۾�. ���� TakeDamage�Լ��� �ߵ� �Ǵ� ����.
					UGameplayStatics::ApplyDamage(Main, SkillDamage, SkillInstigator, this, DamageTypeClass);
					UE_LOG(LogTemp, Warning, TEXT("Main take damage!"));
				}

				// �´� ����Ʈ�� ������ ���� �� ������ ��. 
				// 1. �켱 ������ ���� �� ��ų����Ʈ�� ��ġ�� ������. 
				// 2. �����̹��͸� �ؼ�. ���� �¾��� ����� ����Ʈ�� ����������� 
				if (HitParticle) {
					FVector HittedSkillLocation = GetActorLocation();
					// �ϴ� �� ��ų�� HitParticle�� ���� �Ǿ� �־�� ��! 
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HittedSkillLocation, FRotator(0.f), true); // true�ǹ� : ����Ʈ �ڵ� Destroy
					UE_LOG(LogTemp, Warning, TEXT("Enemy Skill Destroy"));
					
				}
				Main->BeHittedStart();
				/*if (HitSuccessSound) {
					UGameplayStatics::PlaySound2D(this, HitSuccessSound);
				}*/
				Disappear(); // ��ų ����Ʈ ���ְ�
			}
		}
	}
}

void  AEnemySkillBase::HitOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor) {
		// �����ΰ�� 
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
				// ��ų ����Ʈ�� ���̶� �������ȴٸ�! ������ ������ ������ ��.
				SkillDamage = SkillDamageRand();
				if (DamageTypeClass) {
					// ������ ������ ������ �۾�. ���� TakeDamage�Լ��� �ߵ� �Ǵ� ����.
					UGameplayStatics::ApplyDamage(Main, SkillDamage, SkillInstigator, this, DamageTypeClass);
					UE_LOG(LogTemp, Warning, TEXT("Main take damage!"));
				}

				// �´� ����Ʈ�� ������ ���� �� ������ ��. 
				// 1. �켱 ������ ���� �� ��ų����Ʈ�� ��ġ�� ������. 
				// 2. �����̹��͸� �ؼ�. ���� �¾��� ����� ����Ʈ�� ����������� 
				if (HitParticle) {
					FVector HittedSkillLocation = GetActorLocation();
					// �ϴ� �� ��ų�� HitParticle�� ���� �Ǿ� �־�� ��! 
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HittedSkillLocation, FRotator(0.f), true); // true�ǹ� : ����Ʈ �ڵ� Destroy
					UE_LOG(LogTemp, Warning, TEXT("Enemy Skill Destroy"));
				}
				Main->BeHittedStart();
				/*if (HitSuccessSound) {
					UGameplayStatics::PlaySound2D(this, HitSuccessSound);
				}*/
				// Disappear(); // �� ��ų�� �� ĳ������ �����Ǹ� ������ ����
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