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

	// AgroSphere안에 캐릭 들어오면 캐릭 쫒아감
	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(1000.f);
	AgroSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);

	// CombatSphere안에 캐릭 들어오면 캐릭한테 공격 시작함
	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(150.f);

	// 이 범위 안에 Main이 있으면 Main의 컴뱃 타겟을 이놈으로 설정해줌. 보다 원거리에서 Main이 공격할 수 있게 해주기 위함
	SetMainCombatTargetCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SetMainCombatTargetCollision"));
	SetMainCombatTargetCollision->SetupAttachment(GetRootComponent());
	SetMainCombatTargetCollision->InitSphereRadius(1200.f);
	SetMainCombatTargetCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);


	// 공격시 몽타주 노티파이를 이용해 해당 콜리전 활성화 시켜서 데미지 입히기 위한 용도
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

	ItemPercent = 90; // 가장 기본 몹은 아이템 나올 확률이 이것
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// AI컨트롤러 참조를 가지는 작업
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
	// Collision 채널을 바꿔줌 -> 
	// WorldDynamic object that blocks all actors by default. All new custom channels will use its own default response. 
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	CombatCollision2->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	CombatCollision2->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);
	CombatCollision2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// Collision 채널을 바꿔줌 -> 
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
		// OtherActor가 이 범위 안에 들어 왓다면
		// Main만 인식시키기 위한 + 참조하기위한 작업
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
		// OtherActor가 이 범위 안에 들어 왓다면
		// Main만 인식시키기 위한 + 참조하기위한 작업
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
	// 공격범위 안
	if (OtherActor && Alive()) {
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main) {
				if (Main->EquippedWeapon && Main->EquippedWeapon->HitSuccessParticles) {
					// 근거리 공격의 맞았을 때 이펙트효과니까 이건 이 범위 내에서 설정	
					HitParticles = Main->EquippedWeapon->HitSuccessParticles;
				}
				bHasValidTarget = true;
				CombatTarget = Main;
				bOverlappingCombatSphere = true;
				// 상태 변환 해주고 블루프린트에서 공격 애니메이션으로 변경 
				// 상태변환은 Attack()에서 함
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
	// Main의 CombatTarget 세팅에 관련된 모든 것들
	if (OtherActor && Alive()) {
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main) {
				MainReference = Main;
				Main->UpdateCombatTarget(); // 이 함수 하나로 Main->SetCombatTarget(this); // 보간을 하기 위한 조건
											//Main->SetHasCombatTarget(true); // 위젯 표시 여부 설정할 때마다 설정해주기 두 개를 모두 해줌
				if (Main->CombatTarget == this) bIsMainCombatTargetMe = true;
				else bIsMainCombatTargetMe = false;
			}
		}
	}
}

void AEnemy::SetMainCombatTargetCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Main의 CombatTarget 세팅에 관련된 모든 것들
	if (OtherActor) {
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main) {
			if (MainReference) MainReference = nullptr;
			if (Main->CombatTarget == this) {
				// 위의 조건으로 인해 타겟이 여럿일 때 벗어난 놈에 한해서만 보간 안함
				Main->SetCombatTarget(nullptr); // 보간 하지않기 위한 조건
			}
			Main->SetHasCombatTarget(false); // 적 체력위젯 안보이게 하기 위한.

			if (Main->CombatTarget == this) { 
				Main->SetCombatTarget(nullptr);
				Main->bHasCombatTarget = false;
			}
			Main->UpdateCombatTarget(); // 가까운 적 업데이트

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
		// AI로 Move To 하는 작업은 내부적으로 복잡하다 이해가 안가면 un4 문서 참조할 것 

		// 이동에 대한 세부 정보
		FAIMoveRequest MoveRequest;
		// 아래 두개를 제외한 나머지는 Default
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(10.0f); // 캐릭과 25unit거리가 되면 이동 완료로 간주

		// 할당된 경로로 채워진 선택적 출력 매개변수
		FNavPathSharedPtr NavPath;

		AIController->MoveTo(MoveRequest, &NavPath);

		// TArray<FNavPathPoint>  PathPoint = NavPath->GetPathPoints();
		// == auto PathPoint = NavPath->GetPathPoints();

		/**
		* 이동경로 확인용 코드
		* 이동경로 ? AI가 나를 따라올때 어떤 경로로 오는가? DrawDebug해서 보면 직선 경로로 옴
		* 
		for (FNavPathPoint Point : PathPoint) {
			

			// 여기서 Point는 PathPoint배열의 각 요소임. 
			// Point는 내부적으로 구조체로 구성되어 있고 그 중 우리는 FVector Location이 필요

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
				// 마지막 매개변수를 true로 한다면? 계속 발생인가?
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
				// 몬스터가 캐릭한테 데미지 입히는 함수. 매개변수 잘 이해하고 적용하기
				// 3번째 매개변수 AController* EventInstigator : Controller that was responsible for causing this damage (e.g. player who shot the weapon)
				// 5번째 매개변수 UDamageType DamageTypeClass : Class that describes the damage that was done.
				UGameplayStatics::ApplyDamage(Main, Damage, AIController, this, DamageTypeClass);
				// ApplyDamage 함수는 발동순간 데미지 받는 주체의 TakeDamage함수를 발동시킨다.  * TakeDamage()는 APawn의 기본 내장함수
			}
		}
	}
}

void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}


void AEnemy::ActivateCollision()
{
	// 애니메이션 블루프린트에서 사용하기 위한 함수
	// 근거리 공격 몬스터만 적용
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CombatCollision2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateCollision()
{
	// 애니메이션 블루프린트에서 사용하기 위한 함수
	// 근거리 공격 몬스터만 적용
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
			// 공격 중이지도, 맞는 애니메이션 중이지도 아닌 상태
			bAttacking = true;
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance && CombatTarget) {
				if (!IsBoss) {
					// 일반몹이라면
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
					// 일반몹이라면
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
					// 보스라면
					int32 Section = FMath::RandRange(0, 7);
					switch (Section)
					{
					case 0:
						// 전방이동
						UE_LOG(LogTemp, Warning, TEXT("Skill 1"));
						AnimInstance->Montage_Play(CombatMontage, 1.35f);
						AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
						if (Skill_1) {
							FTransform SkillTransForm = FTransform(GetActorRotation(), GetMesh()->GetSocketLocation("BuffSkillLocation"), GetActorScale3D());
							GetWorld()->SpawnActor<AEnemySkillBase>(Skill_1, SkillTransForm);
						}
						break;
					case 1:
						// 캐릭터 위치로 스폰 + 전방이동
						// 위의 내용 수정 요함
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
						// ㄱㅊ
						UE_LOG(LogTemp, Warning, TEXT("Skill 3"));
						AnimInstance->Montage_Play(CombatMontage, 1.35f);
						AnimInstance->Montage_JumpToSection(FName("Attack_3"), CombatMontage);
						if (Skill_3) {
							FTransform SkillTransForm = FTransform(GetActorRotation(), GetMesh()->GetSocketLocation("BuffSkillLocation"), GetActorScale3D());
							GetWorld()->SpawnActor<AEnemySkillBase>(Skill_3, SkillTransForm);
						}
						break;
					case 3:
						// 캐릭터 위치로 스폰 + 전방이동
						// 위의 내용 수정 요함
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
						// 캐릭터 위치로 스폰 + 전방이동
						// 위의 내용 수정 요함
						UE_LOG(LogTemp, Warning, TEXT("Skill 5 : Need to Under Helth 76%"));
						if (Health >= MaxHealth / 1.3f) {
							UE_LOG(LogTemp, Warning, TEXT("Health better than condition"));
							AttackEnd();
							break; // 체력 76% 이하부터 쓸 수 있는 스킬
						}
						else {
							// 체력이 일정수준 이하임. 
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
						// 힐 ㄱㅊ 그리고 걍 여기서 체력 채워주는거 설정해주자 바로
						UE_LOG(LogTemp, Warning, TEXT("Skill 6 : Need to Under Helth 66%"));
						if (Health >= MaxHealth / 1.5f) {
							UE_LOG(LogTemp, Warning, TEXT("Health better than condition"));
							AttackEnd();
							break; //  체력 66% 이하부터 쓸 수 있는 스킬
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
						// ㄱㅊ
						UE_LOG(LogTemp, Warning, TEXT("Skill 7 : Need to Under Helth 50%"));
						if (Health >= MaxHealth / 2.f) {
							UE_LOG(LogTemp, Warning, TEXT("Health better than condition"));
							AttackEnd();
							break; // 체력 절반 이하부터 쓰는 스킬
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
						// ㄱㅊ
						UE_LOG(LogTemp, Warning, TEXT("Skill 8 : Need to Under Helth 30%"));
						if (Health >= MaxHealth / 3.f) {
							UE_LOG(LogTemp, Warning, TEXT("Health better than condition"));
							AttackEnd();
							break; // 체력 1/3 이하부터 쓰는 스킬
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
		// 또한 우리는 만약 캐릭이 공격범위에서 벗어나면 타이머를 리셋해버리고 싶다. 
		// -> CombatSphereOnOverlapEnd에서 설정
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
		여기 작성하다가 작업 끝냄!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
		MainChar->UpdateCombatTarget(); // 가까운 적 업데이트 시켜주기
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
	// 보스이면 죽음 이펙트 설정해주기
	if (DeathEffect) {
		FTransform SkillTransForm = FTransform(GetActorRotation(), GetMesh()->GetSocketLocation("BuffSkillLocation"), GetActorScale3D());
		GetWorld()->SpawnActor<AEnemySkillBase>(DeathEffect, SkillTransForm);
	}
}

void AEnemy::BeHittedStart()
{
	if (Alive()) {
		// 살아있는 상태여야함
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
				// 차라리 확률로 가자
				if (RandNum <= ItemPercent) {
					if (DropItemStorage->DropItemMap.Contains(DropItemBpName[RandIndex])) {
						// 자자. Enemy에 저장되어있는 드랍아이템 목록중에 내가 엔진에서 직접 작성한 드랍아이템이 이름이 일치한다면, 즉 찾은거야 떨굴 아이템을
						AInventory_Pickup* DroppingItem = GetWorld()->SpawnActor<AInventory_Pickup>(DropItemStorage->DropItemMap[DropItemBpName[RandIndex]], SpawnTransform);
						if (DroppingItem) {
							// 아이템 몹 주위로 떨어트리는 작업
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