// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Main.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"
#include "Engine/SkeletalMeshSocket.h"

AWeapon::AWeapon()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetRootComponent());

	// �����ϰ� ������ ��ƼŬ �ý��� ���⿡ �����Ű�� ������ true�� �ٲ��ָ��
	bWeaponParticle = true;

	// ����Ʈ WeaponState ����
	WeaponState = EWeaponState::EWS_Pickup; 

	Damage = 25.f;
	
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// Collision ä���� �ٲ��� -> 
	// WorldDynamic object that blocks all actors by default. All new custom channels will use its own default response. 
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if ((WeaponState == EWeaponState::EWS_Pickup) && OtherActor) {
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main) {
			Main->SetActiveOverlappingItem(this);
		}
	}
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (OtherActor) {
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main) {
			Main->SetActiveOverlappingItem(nullptr);
		}
	}
}

void AWeapon::Equip(class AMain* Char)
{
	if (Char) {
		SetInstigator(Char->GetController());

		// ���� ������ �������ν� ī�޶� ���Ⱑ �����϶� ���� Ȯ��ǰų� ���� ����
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		// ������ �������� ���� false��
		SkeletalMesh->SetSimulatePhysics(false);
		// �Ʒ��� �����������ν� ���⸦ ���¼��� �� ����� ���̻� overlap���� �ȵ�. -> ����Ǹ� ���� �ι������� ����� overlap �ν��ؼ� ���Ⱑ �ı���...
		

		// ĳ���� �� ���Ͽ� ����
		const USkeletalMeshSocket* RighthandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket");
		if (RighthandSocket) {
			// ����
			RighthandSocket->AttachActor(this, Char->GetMesh());
			bRotate = false; // true�� �ϸ� �����ϰ��� ��� ���ư�

			WeaponState = EWeaponState::EWS_Equipped;
			Char->SetEquippedWeapon(this); // this = ���� �� weapon 
			Char->SetActiveOverlappingItem(nullptr);
		}
		if (OnEquipSound) {
			// ������ ����ť�� ������ 
			UGameplayStatics::PlaySound2D(this, OnEquipSound);
		}
		if (!bWeaponParticle) {
			// bWeaponParticle�� false��� ��ƼŬ ��Ȱ��ȭ ��Ŵ
			IdleParticlesComponent->Deactivate();
		}
		else {
			IdleParticlesComponent->Activate();
		}
	}
}

void AWeapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (OtherActor) {
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy) {
			if (Enemy->HitParticles) {
				// ������ �Ű������� true�� �Ѵٸ�? ��� �߻��ΰ�?
				const USkeletalMeshSocket* WeaponSocket = SkeletalMesh->GetSocketByName("WeaponSocket");
				if (WeaponSocket) {
					FVector ParticleLocation = WeaponSocket->GetSocketLocation(SkeletalMesh);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, ParticleLocation, FRotator(0.f), true);
				}
			}
			if (Enemy->HitSound) {
				UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
			}
			if (DamageTypeClass) {
				// ���� �������� �������� ����. main���� ����x ���⿡�� �����ϴ°���
				UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponInstigator, this, DamageTypeClass);
			}
			UE_LOG(LogTemp, Warning, TEXT("Cast to Enemy Success!!!"));
		}
	}
}

void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}


void AWeapon::ActivateCollision()
{
	// �ִϸ��̼� �������Ʈ���� ����ϱ� ���� �Լ�
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::DeactivateCollision()
{
	// �ִϸ��̼� �������Ʈ���� ����ϱ� ���� �Լ�
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

