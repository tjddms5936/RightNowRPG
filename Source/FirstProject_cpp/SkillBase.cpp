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
	ProjectileMovement->ProjectileGravityScale = 0.f; // 0�̻��̸� ���ٰ� ������ 

	HitCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("HitCollision"));
	HitCollision->SetupAttachment(GetRootComponent());

	
	DestroyDelay = 3.f;

	SkillDamage = 10.f;
	MinDmg = 10.f;
	MaxDmg = 10.f;

	// �������� ĳ���� ���󰡸� true�� ����������
	bFromChar = false;
}

// Called when the game starts or when spawned
void ASkillBase::BeginPlay()
{
	Super::BeginPlay();
	/* �� �ؿ� �κ���  �Ļ� ��ų Ŭ�������� ���� ���������� */

	// ���Ϳ��� ���� �ʾƵ� 3�ʵڸ� ������ ���������� ����.
	GetWorldTimerManager().SetTimer(DestroyTime, this, &ASkillBase::Disappear, DestroyDelay);

	// HitCollision �ݸ��� ����. Enemy���� �� Ignore. 
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
		case 1: // ������ų�̶� Begin�ν��� �ȵ�.
			break;
		case 2: // ���Ÿ� ������ų(���׿�) Destroy�� ���⼭ Ÿ�̸ӷ� ����
			if (Enemy) { // ��ų�� �������� ���̱⸸ �ϸ� ��
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
					// 1/3 Ȯ���� ���� �´� ��� ����. 
					Enemy->BeHittedStart(); // BeHittedEnd() �Լ��� �ִϸ��̼�BP���� ��Ƽ���� �̺�Ʈ ���۵Ǹ� ȣ��ǵ��� ����
				}
				// Ÿ�̸ӷ� ��ųActor �ı�
				GetWorldTimerManager().SetTimer(DestroyTime, this, &ASkillBase::Disappear, DestroyDelay);
			}
			break;

		case 3: // ���Ÿ� ���Ͻ�ų(��������Ʈ) Destroy�� ���⼭ ����
			if (Enemy && Enemy->MainReference) { // ��������Ʈ�� ��Ҵµ� ���� �ְ�, Main ĳ���͵� �� ���� ���Ÿ� ���� �ȿ� ���� �ִٸ�
				if (Enemy->MainReference->CombatTarget == Enemy) { // �� �� �� Main ĳ������ Ÿ���õ� ���� �� ��������Ʈ�� ���� ���̶� ���ٸ� 
					SkillDamage = SkillDamageRand();
					if (DamageTypeClass) {
						UGameplayStatics::ApplyDamage(Enemy, SkillDamage, SkillInstigator, this, DamageTypeClass);
					}
					if (HitParticle) {
						FVector HittedSkillLocation = GetActorLocation(); // ���� �´� ������ �� ��ġ
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HittedSkillLocation);
					}
					if (SkillHittedSound) {
						UGameplayStatics::PlaySound2D(this, SkillHittedSound);
					}
					float RandHitted = FMath::RandRange(1, 3);
					if (RandHitted == 1) {
						// 1/3 Ȯ���� ���� �´� ��� ����. 
						Enemy->BeHittedStart(); // BeHittedEnd() �Լ��� �ִϸ��̼�BP���� ��Ƽ���� �̺�Ʈ ���۵Ǹ� ȣ��ǵ��� ����
					}
					// ���ڸ��� ��ų�� �ı���
					Disappear();
				}
			}
			break;

		case 4:
			// 4,5���� ���ֺ� ���� ��ų�̶� Begin�ν��� �ȵ�. ^^ �ٷ� break
			if (Enemy) { // ������ų�̶� �ϴ� ������ ���̱⸸ �ϸ� ��
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
					// 1/3 Ȯ���� ���� �´� ��� ����. 
					Enemy->BeHittedStart(); // BeHittedEnd() �Լ��� �ִϸ��̼�BP���� ��Ƽ���� �̺�Ʈ ���۵Ǹ� ȣ��ǵ��� ����
				}
				// Ÿ�̸ӷ� ��ųActor �ı�
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
		case 1: // ������ų �ν��ؾߵ�
			if (Main) {
				CustomBuffSkill();
			}
			break;
		case 2: // ���Ÿ� ������ų(���׿�) End �ν� �ʿ� x
			break;

		case 3: // ���Ÿ� ���Ͻ�ų(��������Ʈ) End �ν� �ʿ� x
			break;

		case 4:
			// 4,5���� ���ֺ� ���� ��ų�̶� �����Ǹ� �����־������ �����δ� ������. End�ɶ� �ð� ������ �༭ Destroy�ָ� ��
			//if (Enemy) { // ������ų�̶� �ϴ� ������ ���̱⸸ �ϸ� ��
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
			//		// 1/3 Ȯ���� ���� �´� ��� ����. 
			//		Enemy->BeHittedStart(); // BeHittedEnd() �Լ��� �ִϸ��̼�BP���� ��Ƽ���� �̺�Ʈ ���۵Ǹ� ȣ��ǵ��� ����
			//	}
			//	// Ÿ�̸ӷ� ��ųActor �ı�
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
						// 1/3 Ȯ���� ���� �´� ��� ����. 
						Enemy->BeHittedStart(); // BeHittedEnd() �Լ��� �ִϸ��̼�BP���� ��Ƽ���� �̺�Ʈ ���۵Ǹ� ȣ��ǵ��� ����
					}
					// Ÿ�̸ӷ� ��ųActor �ı�
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
		if (bIsBuff) return 1; // ������ų 
		else {
			if (bIsWideDmg) return 2; // ���Ÿ� ������ų(���׿�)
			else return 3; // ���Ÿ� ���� ��ų(������ ��Ʈ)
		}
	}
	else {
		if (bIsWideDmg) return 4; // ���ֺ� ���� ������ų(��Ȧ)
		else return 5; // ���ֺ� ���� ���Ͻ�ų(����?)
	}
}





void ASkillBase::Disappear() {
	/*
	�������Ʈ���� ȣ�� �����ϸ�, ��ų Destroy�� ��ų �������� Ÿ�̹� �����ϰ� ������.
	���� ��ų : �� �ֺ����� ����(c++����) && �����ð� �帥�� Destroy(�������Ʈ ����) && Enemy || Boss �� �� ������ ����(c++����)
	���� ��ų + ���ֺ� : �� �ֺ����� ����(c++����) && �����ð� �帥�� Destroy(�������Ʈ ����) && CombatTarget�� ������ ����(c++����)
	���� ��ų + ĳ���ͷκ��� : ĳ���Ϳ������� ����(c++����) && ������ Destroy(�������Ʈ ����) && Enemy || Boss �� ������ ����
	Buff ��ų : ĳ���Ϳ������� ����(c++����) && ������ ���ִ°� �ƴ϶� Attach���� ĳ���Ϳ�(c++����) && �׳� ���� �� c++ �����ұ�? ����..;;;

	4���� ���ǿ� ���� ������ ���ָ� �Ǵϱ� !!!!
	*/ 
	
	Destroy();
}

float ASkillBase::SkillDamageRand()
{
	return FMath::RandRange(this->MinDmg, this->MaxDmg);
}

