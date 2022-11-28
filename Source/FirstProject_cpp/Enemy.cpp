// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "Main.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Sound/SoundCue.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshsocket.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "MainPlayerController.h"
#include "Weapon.h"
#include "EnemySkillBase.h"
#include "ItemStorage.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// AgroSphere�ȿ� ĳ�� ������ ĳ�� �i�ư�
	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(1000.f);
	AgroSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);

	// CombatSphere�ȿ� ĳ�� ������ ĳ������ ���� ������
	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(150.f);

	// �� ���� �ȿ� Main�� ������ Main�� �Ĺ� Ÿ���� �̳����� ��������. ���� ���Ÿ����� Main�� ������ �� �ְ� ���ֱ� ����
	SetMainCombatTargetCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SetMainCombatTargetCollision"));
	SetMainCombatTargetCollision->SetupAttachment(GetRootComponent());
	SetMainCombatTargetCollision->InitSphereRadius(1200.f);
	SetMainCombatTargetCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);


	// ���ݽ� ��Ÿ�� ��Ƽ���̸� �̿��� �ش� �ݸ��� Ȱ��ȭ ���Ѽ� ������ ������ ���� �뵵
	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetMesh(), FName("EnemySocket"));

	CombatCollision2 = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision2"));
	CombatCollision2->SetupAttachment(GetMesh(), FName("EnemySocket2"));

	bOverlappingCombatSphere = false;

	Health = 75.f;
	MaxHealth = 100.f; 
	Damage = 10.f;

	AttackMinTime = 0.5f;
	AttackMaxTime = 3.5f;

	EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;

	DeathDelay = 3.f;

	bHasValidTarget = false;

	IsBoss = false;

	SpawnSkillLocation = GetActorLocation();

	bHitted = false;

	ItemPercent = 90; // ���� �⺻ ���� ������ ���� Ȯ���� �̰�
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// AI��Ʈ�ѷ� ������ ������ �۾�
	AIController = Cast<AAIController>(GetController());

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);


	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);

	SetMainCombatTargetCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::SetMainCombatTargetCollisionBeginOverlap);
	SetMainCombatTargetCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::SetMainCombatTargetCollisionEndOverlap);


	
	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// Collision ä���� �ٲ��� -> 
	// WorldDynamic object that blocks all actors by default. All new custom channels will use its own default response. 
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	CombatCollision2->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	CombatCollision2->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);
	CombatCollision2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// Collision ä���� �ٲ��� -> 
	// WorldDynamic object that blocks all actors by default. All new custom channels will use its own default response. 
	CombatCollision2->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision2->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision2->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && Alive()) {
		// OtherActor�� �� ���� �ȿ� ��� �Ӵٸ�
		// Main�� �νĽ�Ű�� ���� + �����ϱ����� �۾�
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main) {
			UE_LOG(LogTemp, Warning, TEXT("Main cast success"));
			MoveToTarget(Main);
		}
	}
}

void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor) {
		// OtherActor�� �� ���� �ȿ� ��� �Ӵٸ�
		// Main�� �νĽ�Ű�� ���� + �����ϱ����� �۾�
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main) {
			bHasValidTarget = false;
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			if (AIController) {
				AIController->StopMovement();
			}
		}
	}
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// ���ݹ��� ��
	if (OtherActor && Alive()) {
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main) {
				if (Main->EquippedWeapon && Main->EquippedWeapon->HitSuccessParticles) {
					// �ٰŸ� ������ �¾��� �� ����Ʈȿ���ϱ� �̰� �� ���� ������ ����	
					HitParticles = Main->EquippedWeapon->HitSuccessParticles;
				}
				bHasValidTarget = true;
				CombatTarget = Main;
				bOverlappingCombatSphere = true;
				// ���� ��ȯ ���ְ� �������Ʈ���� ���� �ִϸ��̼����� ���� 
				// ���º�ȯ�� Attack()���� ��
				if (!bHitted && !bAttacking) {
					float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
					GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
				}
			}
		}
	}
}

void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
	if (OtherActor && OtherComp) {
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main) {
				bOverlappingCombatSphere = false;
				MoveToTarget(Main);
				CombatTarget = nullptr;
				GetWorldTimerManager().ClearTimer(AttackTimer);
			}
		}
	}
}

void AEnemy::SetMainCombatTargetCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Main�� CombatTarget ���ÿ� ���õ� ��� �͵�
	if (OtherActor && Alive()) {
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main) {
				MainReference = Main;
				Main->UpdateCombatTarget(); // �� �Լ� �ϳ��� Main->SetCombatTarget(this); // ������ �ϱ� ���� ����
											//Main->SetHasCombatTarget(true); // ���� ǥ�� ���� ������ ������ �������ֱ� �� ���� ��� ����
				if (Main->CombatTarget == this) bIsMainCombatTargetMe = true;
				else bIsMainCombatTargetMe = false;
			}
		}
	}
}

void AEnemy::SetMainCombatTargetCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Main�� CombatTarget ���ÿ� ���õ� ��� �͵�
	if (OtherActor) {
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main) {
			if (MainReference) MainReference = nullptr;
			if (Main->CombatTarget == this) {
				// ���� �������� ���� Ÿ���� ������ �� ��� �� ���ؼ��� ���� ����
				Main->SetCombatTarget(nullptr); // ���� �����ʱ� ���� ����
			}
			Main->SetHasCombatTarget(false); // �� ü������ �Ⱥ��̰� �ϱ� ����.

			if (Main->CombatTarget == this) { 
				Main->SetCombatTarget(nullptr);
				Main->bHasCombatTarget = false;
			}
			Main->UpdateCombatTarget(); // ����� �� ������Ʈ

			if (Main->MainPlayerController) {
				USkeletalMeshComponent* MainMesh = Cast<USkeletalMeshComponent>(OtherComp);
				if (MainMesh) {
					Main->MainPlayerController->RemoveEnemyHealthBar();
				}
			}
		}

		
	}
}

void AEnemy::MoveToTarget(AMain* Target)
{
	UE_LOG(LogTemp, Warning, TEXT("Move to Target Start"));
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
	if (AIController) {
		UE_LOG(LogTemp, Warning, TEXT("AIController exist"));
		// AI�� Move To �ϴ� �۾��� ���������� �����ϴ� ���ذ� �Ȱ��� un4 ���� ������ �� 

		// �̵��� ���� ���� ����
		FAIMoveRequest MoveRequest;
		// �Ʒ� �ΰ��� ������ �������� Default
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(10.0f); // ĳ���� 25unit�Ÿ��� �Ǹ� �̵� �Ϸ�� ����

		// �Ҵ�� ��η� ä���� ������ ��� �Ű�����
		FNavPathSharedPtr NavPath;

		AIController->MoveTo(MoveRequest, &NavPath);

		// TArray<FNavPathPoint>  PathPoint = NavPath->GetPathPoints();
		// == auto PathPoint = NavPath->GetPathPoints();

		/**
		* �̵���� Ȯ�ο� �ڵ�
		* �̵���� ? AI�� ���� ����ö� � ��η� ���°�? DrawDebug�ؼ� ���� ���� ��η� ��
		* 
		for (FNavPathPoint Point : PathPoint) {
			

			// ���⼭ Point�� PathPoint�迭�� �� �����. 
			// Point�� ���������� ����ü�� �����Ǿ� �ְ� �� �� �츮�� FVector Location�� �ʿ�

			FVector Location = Point.Location;
			UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Green, 10.f, 2.5f);
		}
		*/
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Not exist AIController"));
	}

}

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor) {
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main) {
			if (Main->HitParticles) {
				// ������ �Ű������� true�� �Ѵٸ�? ��� �߻��ΰ�?
				const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocket");
				const USkeletalMeshSocket* TipSocket2 = GetMesh()->GetSocketByName("TipSocket2");
				if (TipSocket) {
					FVector ParticleLocation = TipSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, ParticleLocation, FRotator(0.f), true);
				}
				else if (TipSocket2) {
					FVector ParticleLocation = TipSocket2->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, ParticleLocation, FRotator(0.f), true);
				}
			}
			if (Main->HitSound) {
				UGameplayStatics::PlaySound2D(this, Main->HitSound);
			}
			if (DamageTypeClass) {
				// ���Ͱ� ĳ������ ������ ������ �Լ�. �Ű����� �� �����ϰ� �����ϱ�
				// 3��° �Ű����� AController* EventInstigator : Controller that was responsible for causing this damage (e.g. player who shot the weapon)
				// 5��° �Ű����� UDamageType DamageTypeClass : Class that describes the damage that was done.
				UGameplayStatics::ApplyDamage(Main, Damage, AIController, this, DamageTypeClass);
				// ApplyDamage �Լ��� �ߵ����� ������ �޴� ��ü�� TakeDamage�Լ��� �ߵ���Ų��.  * TakeDamage()�� APawn�� �⺻ �����Լ�
			}
		}
	}
}

void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}


void AEnemy::ActivateCollision()
{
	// �ִϸ��̼� �������Ʈ���� ����ϱ� ���� �Լ�
	// �ٰŸ� ���� ���͸� ����
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CombatCollision2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateCollision()
{
	// �ִϸ��̼� �������Ʈ���� ����ϱ� ���� �Լ�
	// �ٰŸ� ���� ���͸� ����
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Attack()
{
	if (Alive() && bHasValidTarget) {
		if (AIController) {
			AIController->StopMovement();
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
		}
		if (!bAttacking && !bHitted) {
			// ���� ��������, �´� �ִϸ��̼� �������� �ƴ� ����
			bAttacking = true;
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance && CombatTarget) {
				if (!IsBoss) {
					// �Ϲݸ��̶��
					int32 Section = FMath::RandRange(0, 2);
					switch (Section)
					{
					case 0:
						AnimInstance->Montage_Play(CombatMontage, 1.35f);
						AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);
						break;
					case 1:
						AnimInstance->Montage_Play(CombatMontage, 1.35f);
						AnimInstance->Montage_JumpToSection(FName("Attack2"), CombatMontage);
						break;
					case 2:
						AnimInstance->Montage_Play(CombatMontage, 1.35f);
						AnimInstance->Montage_JumpToSection(FName("Attack3"), CombatMontage);
						break;
					default:
						;
					}
				}
				else if (IsBoss && bIsLevel1Boss || bIsLevel2Boss || bIsLevel3Boss) {
					// �Ϲݸ��̶��
					int32 Section = FMath::RandRange(0, 2);
					switch (Section)
					{
					case 0:
						AnimInstance->Montage_Play(CombatMontage, 1.35f);
						AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);
						break;
					case 1:
						AnimInstance->Montage_Play(CombatMontage, 1.35f);
						AnimInstance->Montage_JumpToSection(FName("Attack2"), CombatMontage);
						break;
					case 2:
						AnimInstance->Montage_Play(CombatMontage, 1.35f);
						AnimInstance->Montage_JumpToSection(FName("Attack3"), CombatMontage);
						break;
					default:
						;
					}
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("This is Boss"));
					// �������
					int32 Section = FMath::RandRange(0, 7);
					switch (Section)
					{
					case 0:
						// �����̵�
						UE_LOG(LogTemp, Warning, TEXT("Skill 1"));
						AnimInstance->Montage_Play(CombatMontage, 1.35f);
						AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
						if (Skill_1) {
							FTransform SkillTransForm = FTransform(GetActorRotation(), GetMesh()->GetSocketLocation("BuffSkillLocation"), GetActorScale3D());
							GetWorld()->SpawnActor<AEnemySkillBase>(Skill_1, SkillTransForm);
						}
						break;
					case 1:
						// ĳ���� ��ġ�� ���� + �����̵�
						// ���� ���� ���� ����
						UE_LOG(LogTemp, Warning, TEXT("Skill 2"));
						AnimInstance->Montage_Play(CombatMontage, 1.35f);
						AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
						if (CombatTarget) {
							SpawnSkillLocation = CombatTarget->GetMesh()->GetSocketLocation("EnemySkillSpawnPoint");
						}
						if (Skill_2) {
							FTransform SkillTransForm = FTransform(GetActorRotation(), SpawnSkillLocation, GetActorScale3D());
							GetWorld()->SpawnActor<AEnemySkillBase>(Skill_2, SkillTransForm);
						}
						break;
					case 2:
						// ����
						UE_LOG(LogTemp, Warning, TEXT("Skill 3"));
						AnimInstance->Montage_Play(CombatMontage, 1.35f);
						AnimInstance->Montage_JumpToSection(FName("Attack_3"), CombatMontage);
						if (Skill_3) {
							FTransform SkillTransForm = FTransform(GetActorRotation(), GetMesh()->GetSocketLocation("BuffSkillLocation"), GetActorScale3D());
							GetWorld()->SpawnActor<AEnemySkillBase>(Skill_3, SkillTransForm);
						}
						break;
					case 3:
						// ĳ���� ��ġ�� ���� + �����̵�
						// ���� ���� ���� ����
						UE_LOG(LogTemp, Warning, TEXT("Skill 4"));
						AnimInstance->Montage_Play(CombatMontage, 1.35f);
						AnimInstance->Montage_JumpToSection(FName("Attack_4"), CombatMontage);
						if (CombatTarget) {
							SpawnSkillLocation = CombatTarget->GetMesh()->GetSocketLocation("EnemySkillSpawnPoint");
						}
						if (Skill_4) {
							FTransform SkillTransForm = FTransform(GetActorRotation(), SpawnSkillLocation, GetActorScale3D());
							GetWorld()->SpawnActor<AEnemySkillBase>(Skill_4, SkillTransForm);
						}
						break;
					case 4:
						// ĳ���� ��ġ�� ���� + �����̵�
						// ���� ���� ���� ����
						UE_LOG(LogTemp, Warning, TEXT("Skill 5 : Need to Under Helth 76%"));
						if (Health >= MaxHealth / 1.3f) {
							UE_LOG(LogTemp, Warning, TEXT("Health better than condition"));
							AttackEnd();
							break; // ü�� 76% ���Ϻ��� �� �� �ִ� ��ų
						}
						else {
							// ü���� �������� ������. 
							AnimInstance->Montage_Play(CombatMontage, 1.35f);
							AnimInstance->Montage_JumpToSection(FName("Attack_5"), CombatMontage);
							if (CombatTarget) {
								SpawnSkillLocation = CombatTarget->GetMesh()->GetSocketLocation("EnemySkillSpawnPoint");
							}
							if (Skill_5) {
								FTransform SkillTransForm = FTransform(GetActorRotation(), SpawnSkillLocation, GetActorScale3D());
								GetWorld()->SpawnActor<AEnemySkillBase>(Skill_5, SkillTransForm);
							}
							break;
						}
					case 5:
						// �� ���� �׸��� �� ���⼭ ü�� ä���ִ°� ���������� �ٷ�
						UE_LOG(LogTemp, Warning, TEXT("Skill 6 : Need to Under Helth 66%"));
						if (Health >= MaxHealth / 1.5f) {
							UE_LOG(LogTemp, Warning, TEXT("Health better than condition"));
							AttackEnd();
							break; //  ü�� 66% ���Ϻ��� �� �� �ִ� ��ų
						}
						else {
							AnimInstance->Montage_Play(CombatMontage, 1.35f);
							AnimInstance->Montage_JumpToSection(FName("Attack_6"), CombatMontage);
							if (Skill_6) {
								FTransform SkillTransForm = FTransform(GetActorRotation(), GetMesh()->GetSocketLocation("BuffSkillLocation"), GetActorScale3D());
								GetWorld()->SpawnActor<AEnemySkillBase>(Skill_6, SkillTransForm);
								if (Health += (Health * 1.5f) >= MaxHealth) {
									Health = MaxHealth;
								}
								else {
									Health += (Health * 1.5f);
								}
							}
							break;
						}
					case 6:
						// ����
						UE_LOG(LogTemp, Warning, TEXT("Skill 7 : Need to Under Helth 50%"));
						if (Health >= MaxHealth / 2.f) {
							UE_LOG(LogTemp, Warning, TEXT("Health better than condition"));
							AttackEnd();
							break; // ü�� ���� ���Ϻ��� ���� ��ų
						}
						else {
							AnimInstance->Montage_Play(CombatMontage, 1.35f);
							AnimInstance->Montage_JumpToSection(FName("Attack_7"), CombatMontage);
							if (Skill_7) {
								FTransform SkillTransForm = FTransform(GetActorRotation(), GetMesh()->GetSocketLocation("BuffSkillLocation"), GetActorScale3D());
								GetWorld()->SpawnActor<AEnemySkillBase>(Skill_7, SkillTransForm);
							}
							break;
						}
					case 7:
						// ����
						UE_LOG(LogTemp, Warning, TEXT("Skill 8 : Need to Under Helth 30%"));
						if (Health >= MaxHealth / 3.f) {
							UE_LOG(LogTemp, Warning, TEXT("Health better than condition"));
							AttackEnd();
							break; // ü�� 1/3 ���Ϻ��� ���� ��ų
						}
						else {
							AnimInstance->Montage_Play(CombatMontage, 1.35f);
							AnimInstance->Montage_JumpToSection(FName("Attack_8"), CombatMontage);
							if (Skill_8) {
								FTransform SkillTransForm = FTransform(GetActorRotation(), GetMesh()->GetSocketLocation("BuffSkillLocation"), GetActorScale3D());
								GetWorld()->SpawnActor<AEnemySkillBase>(Skill_8, SkillTransForm);
							}
							break;
						}
					default:
						;
					}
				}
			}
		}
	}
}

void AEnemy::AttackEnd()
{
	bAttacking = false;
	if (bOverlappingCombatSphere && !bHitted) {
		float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
		// ���� �츮�� ���� ĳ���� ���ݹ������� ����� Ÿ�̸Ӹ� �����ع����� �ʹ�. 
		// -> CombatSphereOnOverlapEnd���� ����
	}
}

void AEnemy::PlayAttackSound()
{
	if (AttackSound) {
		UGameplayStatics::PlaySound2D(this, AttackSound);
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID)) {
		/*
		* =======================================================================
		���� �ۼ��ϴٰ� �۾� ����!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		==========================================================================
		*/
	}
	if (Health - DamageAmount <= 0.f) {
		Health = 0.f;
		Die(DamageCauser);
	}
	else {
		Health -= DamageAmount;
	}
	return DamageAmount;
}

void AEnemy::ReceiveRadialDamage(float DamageReceived, const UDamageType* DamageType, FVector Origin, const FHitResult& HitInfo, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health - DamageReceived <= 0.f) {
		Health = 0.f;
		Die(DamageCauser);
	}
	else {
		Health -= DamageReceived;
	}
}

void AEnemy::Die(AActor* Causer)
{

	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);

	AMain* MainChar = Cast<AMain>(CombatTarget);
	if (MainChar) {
		MainChar->UpdateCombatTarget(); // ����� �� ������Ʈ �����ֱ�
		if (IsBoss) {

			if (bIsLevel1Boss) {
				MainChar->bIsLevel1Clear = true;
			}
			else if (bIsLevel2Boss) {
				MainChar->bIsLevel2Clear = true;
			}
			else if (bIsLevel3Boss) {
				MainChar->bIsLevel3Clear = true;
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("What Level Boss?"));
			}
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Where is MainChar"));
	}

	SpawnDropItem();


	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance) {
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);
	}

	bAttacking = false;

}

void AEnemy::SetAllCollision_Disabled() 
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::DeathEnd()
{
	 GetMesh()->bPauseAnims = true;
	 GetMesh()->bNoSkeletonUpdate = true;

	// SetAllCollision_Disabled();

	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);

}

bool AEnemy::Alive()
{
	return GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead;
}

void AEnemy::Disappear()
{
	UE_LOG(LogTemp, Warning, TEXT("Disappear() begin"));
	Destroy();
}

void AEnemy::DeathEndEffect()
{
	// �����̸� ���� ����Ʈ �������ֱ�
	if (DeathEffect) {
		FTransform SkillTransForm = FTransform(GetActorRotation(), GetMesh()->GetSocketLocation("BuffSkillLocation"), GetActorScale3D());
		GetWorld()->SpawnActor<AEnemySkillBase>(DeathEffect, SkillTransForm);
	}
}

void AEnemy::BeHittedStart()
{
	if (Alive()) {
		// ����ִ� ���¿�����
		if (AIController) {
			AIController->StopMovement();
		}
		bHitted = true;
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Hitted);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance) {
			AnimInstance->Montage_Play(CombatMontage, 1.5f);
			AnimInstance->Montage_JumpToSection(FName("Hitted"), CombatMontage);
		}
	}
}

void AEnemy::BeHittedEnd()
{
	bHitted = false;
	if (bAttacking) bAttacking = false;
	if (bOverlappingCombatSphere) {
		float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
	}
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
}

void AEnemy::SpawnDropItem()
{
	FVector SpawnLocation = GetMesh()->GetSocketLocation(FName("SpawnDropItemLocation"));
	FTransform SpawnTransform = FTransform(GetActorRotation(), SpawnLocation, GetActorScale3D());
	if (DropItem) {
		AItemStorage* DropItemStorage = GetWorld()->SpawnActor<AItemStorage>(DropItem);

		if (DropItemStorage) {
			if (DropItemBpName.Num() != 0) {
				int32 RandNum = FMath::RandRange(0, 100);
				int32 RandIndex = FMath::RandRange(0, DropItemBpName.Num()-1);
				// ���� Ȯ���� ����
				if (RandNum <= ItemPercent) {
					if (DropItemStorage->DropItemMap.Contains(DropItemBpName[RandIndex])) {
						// ����. Enemy�� ����Ǿ��ִ� ��������� ����߿� ���� �������� ���� �ۼ��� ����������� �̸��� ��ġ�Ѵٸ�, �� ã���ž� ���� ��������
						AInventory_Pickup* DroppingItem = GetWorld()->SpawnActor<AInventory_Pickup>(DropItemStorage->DropItemMap[DropItemBpName[RandIndex]], SpawnTransform);
						if (DroppingItem) {
							// ������ �� ������ ����Ʈ���� �۾�
							DroppingItem->PickupMesh->SetSimulatePhysics(true);
							FVector Impulse(5.f, 0.f, 5.f);
							float RandomRange = FMath::RandRange(0.f, 360.f);
							DroppingItem->PickupMesh->AddImpulse(Impulse.RotateAngleAxis(RandomRange, FVector(0.f, 0.f, 1.f)), NAME_None, true);
						}
					}
				}
				else return;
			}
		}
	}
	else return;
}