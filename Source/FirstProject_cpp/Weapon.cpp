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

	// 장착하고 나서도 파티클 시스템 무기에 적용시키고 싶으면 true로 바꿔주면됨
	bWeaponParticle = true;

	// 디폴트 WeaponState 설정
	WeaponState = EWeaponState::EWS_Pickup; 

	Damage = 25.f;
	
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// Collision 채널을 바꿔줌 -> 
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

		// 밑의 설정을 해줌으로써 카메라가 무기가 움직일때 따라 확대되거나 하지 않음
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		// 무기의 물리적인 설정 false로
		SkeletalMesh->SetSimulatePhysics(false);
		// 아래를 설정해줌으로써 무기를 끼는순간 낀 무기는 더이상 overlap적용 안됨. -> 적용되면 공격 두번누르면 무기랑 overlap 인식해서 무기가 파괴됨...
		

		// 캐릭터 손 소켓에 장착
		const USkeletalMeshSocket* RighthandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket");
		if (RighthandSocket) {
			// 장착
			RighthandSocket->AttachActor(this, Char->GetMesh());
			bRotate = false; // true로 하면 장착하고서도 계속 돌아감

			WeaponState = EWeaponState::EWS_Equipped;
			Char->SetEquippedWeapon(this); // this = 현재 이 weapon 
			Char->SetActiveOverlappingItem(nullptr);
		}
		if (OnEquipSound) {
			// 설정한 사운드큐가 있으면 
			UGameplayStatics::PlaySound2D(this, OnEquipSound);
		}
		if (!bWeaponParticle) {
			// bWeaponParticle가 false라면 파티클 비활성화 시킴
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
				// 마지막 매개변수를 true로 한다면? 계속 발생인가?
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
				// 때린 몬스터한테 데미지를 가함. main에서 구현x 무기에서 구현하는거임
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
	// 애니메이션 블루프린트에서 사용하기 위한 함수
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::DeactivateCollision()
{
	// 애니메이션 블루프린트에서 사용하기 위한 함수
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

