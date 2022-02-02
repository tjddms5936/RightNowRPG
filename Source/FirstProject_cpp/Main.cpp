// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Weapon.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundCue.h"
#include "Enemy.h"
#include "MainPlayerController.h"
#include "FirstSaveGame.h"

// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Create Camera Boom (pulls towards the player if there's a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f; // Camera follows at this distance
	CameraBoom->bUsePawnControlRotation = true; // Rotate arm based on controller

	// Set size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(48.f, 105.f);

	// Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match
	// the controller orientation
	FollowCamera->bUsePawnControlRotation = false;
	// Set our turn rates for input
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	// We don't want to rotate the character along with the rotation
	// Let that just affect the camera.
	// If you want to rotate your character along with camera, All of this value need to change for true.
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;

	// 방향키 누르면 그 방향으로 캐릭터가 봄
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f); // ... at this rotation rate // 얼마나 빠르게 방향회전하는지
	GetCharacterMovement()->JumpZVelocity = 650.f; // Jump 높이 
	GetCharacterMovement()->AirControl = 0.2f; // Character can moving in the air 

	// Default Player Stats
	MaxHealth = 100.f;
	Health = 65.f;
	MaxStamina = 150.f;
	Stamina = 120.f;
	Coins = 0;

	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;
	
	bShiftKeyDown = false;

	// Initialize Enums
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminasStatus::ESS_Normal;

	StaminaDrainRate = 25.f;
	MinSprintStamina = 50.f;
	
	bLMBDown = false;
	IsMoveKeyDown = false;

	InterpSpeed = 15.f;
	bInterpToEnemy = false;

	bHasCombatTarget = false;
	
	bMovingForward = false;
	bMovingRight = false;
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();

	MainPlayerController = Cast<AMainPlayerController>(GetController());
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	float DeltaStamina = StaminaDrainRate * DeltaTime;

	switch (StaminaStatus)
	{
		case EStaminasStatus::ESS_Normal:
			// 스테미너가 정상 상태일 때
			if (bShiftKeyDown) {
				// 달리기 누른 상태에서
				if (Stamina - DeltaStamina <= MinSprintStamina) {
					// 남아있는 스테미나가 최저기준 이하라면
					SetStaminaStatus(EStaminasStatus::ESS_BelowMinimum);
					Stamina -= DeltaStamina;
				}
				else {
					// 최저 이상이라면 스테미나 상태 업데이트 필요 x.
					Stamina -= DeltaStamina;
				}
				// 달리고있는 상태로 변환해주기 
				if (bMovingForward || bMovingRight) {
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				}
				else {
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
			}
			else {
				// 쉬프트 키 땐 상태에서
				if (Stamina + DeltaStamina >= MaxStamina) {
					// 추가하려니 최대치 이상이라면
					Stamina = MaxStamina; // 그냥 최대치로 초기화
				}
				else {
					// 최대치 아니라면 스테미나 회복시켜주기
					Stamina += DeltaStamina;
				}
				// MovementStatus 설정해주기 
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			break;
		case EStaminasStatus::ESS_BelowMinimum:
			// 스테미너 상태가 기준최저인 상태
			if (bShiftKeyDown) {
				// 달리기 누른 상태에서
				if (Stamina - DeltaStamina <= 0.f) {
					// 스테미나 <= 0.f 일때
					SetStaminaStatus(EStaminasStatus::ESS_Exhausted);
					Stamina = 0.f;
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
				else {
					// 0 < 스테미나 <= BelowMinimun 일때
					Stamina -= DeltaStamina;
					if (bMovingForward || bMovingRight) {
						SetMovementStatus(EMovementStatus::EMS_Sprinting);
					}
					else {
						SetMovementStatus(EMovementStatus::EMS_Normal);
					}
				}
			}
			else {
				// 쉬프트 키 땐 상태에서
				if (Stamina + DeltaStamina >= MinSprintStamina) {
					// 추가회복해서 >= Min 이 된다면 
					SetStaminaStatus(EStaminasStatus::ESS_Normal);
					Stamina += DeltaStamina;
				}
				else {
					// 추가 회복 해도 < min 이라면
					Stamina += DeltaStamina;
				}
				// 어떤 상태든 걷는건 동일
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			break;
		case EStaminasStatus::ESS_Exhausted:
			if (bShiftKeyDown) {
				Stamina = 0.f;
			}
			else {
				SetStaminaStatus(EStaminasStatus::ESS_ExhaustedRecovering);
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
			break;
		case EStaminasStatus::ESS_ExhaustedRecovering:
			if (Stamina + DeltaStamina >= MinSprintStamina) {
				SetStaminaStatus(EStaminasStatus::ESS_Normal);
				Stamina += DeltaStamina;
			}
			else {
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
			break;
		default:
			;
	}

	if (bInterpToEnemy && CombatTarget) {
		// 위 두가지 조건이 만족하면 보간 시작 : 적이 나를 돌아보게 만드는 회전보간
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);
		
		SetActorRotation(InterpRotation);
	}

	if (CombatTarget) {
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (MainPlayerController) {
			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}
}

FRotator AMain::GetLookAtRotationYaw(FVector Target)
{
	// FindLookAtRotation : Find a rotation for an object at Start location to point at Target location.
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent); // PlayerInputComponent가 false라면 코드 멈춤

	// 점프
	// IE_Pressed : 눌렀을 때
	// IE_Released : 땟을 때
	// ACharacter를 이미 상속했고 그 안에는 Jump가 구현되어 있음.
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMain::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMain::LMBUp);



	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpAtRate);

}


void AMain::MoveForward(float Value) {
	bMovingForward = false; // 움직이기 전에 false로 초기화
	if (Controller != nullptr && Value != 0.0f && (!bAttacking) && (MovementStatus != EMovementStatus::EMS_Dead) ) {
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		// 카메라 전환되면 자연스럽게 캐릭터도 그 방향 봄
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

		bMovingForward = true; // 움직이는 순간 true로 초기화
	}
}

void AMain::MoveRight(float Value) {
	bMovingRight = false; // 움직이기 전에 false로 초기화
	if (Controller != nullptr && Value != 0.0f && (!bAttacking) && (MovementStatus != EMovementStatus::EMS_Dead)) {
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);

		bMovingRight = true; // 움직이는 순간 true로 초기화
	}
}

void AMain::TurnAtRate(float Rate) {
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LookUpAtRate(float Rate) {
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LMBDown()
{
	bLMBDown = true;

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	if (ActiveOverlappingItem) {
		// ActiveOverlappingItem은 게임시작후 Main누른상태에서 디테일->Item보면
		// ActiveOverlappingItem = 없음 상태임. 그러다가 무기랑 오버렙되면 바뀜
		// 만약 왼쪽마우스를 눌렀는데 아이템과 오버랩 된 상황이라면
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem); // 형변환 하고
		if (Weapon) {
			UE_LOG(LogTemp, Warning, TEXT("Weapon Equip!"));
			Weapon->Equip(this); // 무기 장착! 
			Weapon->SetMainReference(this);
			// 밑에 설정안하면 장착하면 ActiveOverlappingItem가 계속 설정되어있음
			SetActiveOverlappingItem(nullptr); // null로 해줘서 다른 무기도 장착할 수 있게 함
		}
	}

	else if(EquippedWeapon)
	{
		Attack();
	}
}

void AMain::LMBUp()
{
	bLMBDown = false;
}



void AMain::DecrementHealth(float Amount) {
	Health -= Amount;
	if (Health - Amount <= 0.f) {
		Die();
	}
}

void AMain::Die() {
	// 만약 이미 죽은 상태라면 바로 맞고 또 애니메이션 발생하지 않도록 설정
	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage) {
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection("Death", CombatMontage);
	}
	SetMovementStatus(EMovementStatus::EMS_Dead);
}

void AMain::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

void AMain::Jump()
{
	if (MovementStatus != EMovementStatus::EMS_Dead) {
		Super::Jump();
	}
}


void AMain::IncrementCoins(int32 Amount) {
	this->Coins += Amount;
}

void AMain::IncrementHealth(float Amount) {
	if (Health + Amount >= MaxHealth) {
		Health = MaxHealth;
		return;
	}
	Health += Amount;
}

void AMain::IncrementStamina(float Amount)
{
	if (Stamina + Amount >= MaxStamina) {
		Stamina = MaxStamina;
		return;
	}
	Stamina += Amount;
}

void AMain::SetMovementStatus(EMovementStatus Status) {
	MovementStatus = Status;
	if (MovementStatus == EMovementStatus::EMS_Sprinting) {
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else {
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}

void AMain::ShiftKeyDown()
{
	bShiftKeyDown = true;
}

void AMain::ShiftKeyUp()
{
	bShiftKeyDown = false;
}

void AMain::ShowPickupLocation() {
	// Debug 구 형성 
	/*
	UObject* WorldContextObject : 어느 월드?
	FVector const Center : 구 생성 위치는?
	float Radius : 구 반지름은?
	int32 Segments : 구 몇 각형?
	FLinearColor Color : 구 색은?
	float LifeTime : 겜 시작하고 몇초동안 유지?
	float Thickness : 구 두께는?
	*/

	for (int32 i = 0; i < PickupLocations.Num(); i++) {
		UKismetSystemLibrary::DrawDebugSphere(this, PickupLocations[i] + FVector(0, 0, 100.f), 25.f, 8, FLinearColor::Red, 10.f, .5f);
	}
	// 컨테이너에서 쓰는 방법으로 같은 반복문 구현
	for (FVector Location : PickupLocations) {
		// FVector Location = auto Location
		UKismetSystemLibrary::DrawDebugSphere(this, Location + FVector(0, 0, 100.f), 25.f, 8, FLinearColor::Green, 3.f, 2.f);
	}

}

void AMain::SetEquippedWeapon(AWeapon* WeaponToSet)
{
	
	if (EquippedWeapon) {
		// 이미 장착되어있는게 있다면 장착된 걸 Destroy해버림
		 EquippedWeapon->Destroy();
		 UE_LOG(LogTemp, Warning, TEXT("Weapon Destroy"));
	}
	EquippedWeapon = WeaponToSet;
}

void AMain::Attack()
{
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead) {
		bAttacking = true;
		SetInterpToEnemy(true);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage) {

			int32 Section = FMath::RandRange(0, 1);
			switch (Section)
			{
			case 0:
				AnimInstance->Montage_Play(CombatMontage, 2.2f);
				AnimInstance->Montage_JumpToSection("Attack_1", CombatMontage);
				break;
			case 1:
				AnimInstance->Montage_Play(CombatMontage, 1.8f);
				AnimInstance->Montage_JumpToSection("Attack_2", CombatMontage);
				break;
			default:
				;
			}
		}
	}
}

void AMain::AttackEnd()
{
	// 애니메이션 블루프린트에서 사용하기 위한 함수
	bAttacking = false;
	SetInterpToEnemy(false);
	if (bLMBDown) {
		// 왼쪽마우스 버튼 누르고 있는 상태라면 계속 공격
		Attack();
	}
}

void AMain::PlaySwingSound()
{
	if (EquippedWeapon->SwingSound) {
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}

void AMain::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

float AMain::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Health -= DamageAmount;
	if (Health - DamageAmount <= 0.f) {
		Die();
		if (DamageCauser) {
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if (Enemy) {
				Enemy->bHasValidTarget = false;
			}
		}
	}

	return DamageAmount;
}

void AMain::UpdateCombatTarget()
{
	// 가장 가까운 적 찾아주는 함수
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	if (OverlappingActors.Num() == 0) {
		if (MainPlayerController) {
			MainPlayerController->RemoveEnemyHealthBar();
		}
		return;
	}
	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);
	if (ClosestEnemy) {
		float MinDistance = (ClosestEnemy->GetActorLocation() - GetActorLocation()).Size();

		for (auto Actor : OverlappingActors) {
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if (Enemy) {
				float DistanceToActor = (Enemy->GetActorLocation() - GetActorLocation()).Size();
				if (MinDistance > DistanceToActor) {
					MinDistance = DistanceToActor;
					ClosestEnemy = Enemy;
				}
			}
		}
		if (MainPlayerController) {
			MainPlayerController->DisplayEnemyHealthBar();
		}
		SetCombatTarget(ClosestEnemy);
		bHasCombatTarget = true;
	}
}

void AMain::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld();
	if (World) {
		FString CurrentLevel = World->GetMapName();
		// FName을 FString으로 형변환 못함 but 반대는 가능 
		// 이를 위해서 참조 해제 연산자를 사용하셈
		FName CurrentLevelName(*CurrentLevel);
		if (CurrentLevelName != LevelName) {
			UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
}

void AMain::SaveGame()
{
	/*
	UFirstSaveGame::StaticClass() :  USaveGame 포인터가 생김
	 UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass());
	 CreateSaveGameObject : 데이터를 설정할 비어 있는 새 SaveGame 개체를 만든 다음 SaveGameToSlot에 전달합니다.
	*/
	UFirstSaveGame* SaveGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	SaveGameInstance->CharacterStats.Health = Health;
	SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
	SaveGameInstance->CharacterStats.Stamina = Stamina;
	SaveGameInstance->CharacterStats.MaxStamina = MaxStamina;
	SaveGameInstance->CharacterStats.Coins = Coins;
	SaveGameInstance->CharacterStats.Location = GetActorLocation();
	SaveGameInstance->CharacterStats.Rotation = GetActorRotation();

	/*
	게임 내 데이터를 컴퓨터에 저장하기 위함
	SaveGameObject의 내용을 플랫폼별 저장 슬롯/파일에 저장합니다.
	@note 이것은 모든 비일시적 속성을 기록할 것이며 SaveGame 속성 플래그는 확인되지 않습니다
	*/
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex);
}

void AMain::LoadGame(bool SetPosition)
{
	UFirstSaveGame* LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
	Coins = LoadGameInstance->CharacterStats.Coins;

	/*
	다음 맵으로 레벨 이동한다는 것은 Save Load한다는것임. 
	단순하게 다음 레벨로 넘어갈때
	 우리는 위치나 회전값까지 가져올 필요는 없음
	*/
	if (SetPosition) {
		// SetPosition이 true라는 건 레벨이동이 아니라는 것.
		SetActorLocation(LoadGameInstance->CharacterStats.Location);
		SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
	 }

}