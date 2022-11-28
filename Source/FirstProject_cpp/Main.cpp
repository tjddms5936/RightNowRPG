// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Weapon.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundCue.h"
#include "Enemy.h"
#include "MainPlayerController.h"
#include "FirstSaveGame.h"
#include "ItemStorage.h"
#include "CameraShaking.h"
#include "SkillBase.h"
#include "Particles/ParticleSystemComponent.h"
#include "Inventory_Interactable.h"


// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Create Camera Boom (pulls towards the player if there's a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 450.f; // Camera follows at this distance
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
	BaseLookUpRate = 10.f;


	SkillSpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SkillSpawningBox"));
	SkillSpawningBox->SetupAttachment(GetRootComponent());


	// We don't want to rotate the character along with the rotation
	// Let that just affect the camera.
	// If you want to rotate your character along with camera, All of this value need to change for true.
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;

	// ����Ű ������ �� �������� ĳ���Ͱ� ��
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f); // ... at this rotation rate // �󸶳� ������ ����ȸ���ϴ���
	GetCharacterMovement()->JumpZVelocity = 450.f; // Jump ���� 
	GetCharacterMovement()->AirControl = 0.2f; // Character can moving in the air 

	// Default Player Stats
	MaxHealth = 500.f;
	Health = 250.f;
	MaxStamina = 500.f;
	Stamina = 250.f;
	Coins = 0;
	STR = 10; // �⺻ ���ݷ� 10

	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;
	
	bShiftKeyDown = false;

	// Initialize Enums
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminasStatus::ESS_Normal;
	IsSkillStatus = EIsSkillStatus::EMS_Normal;

	StaminaDrainRate = 25.f;
	MinSprintStamina = 50.f;
	
	bLMBDown = false;
	IsMoveKeyDown = false;

	InterpSpeed = 15.f;
	bInterpToEnemy = false;

	bHasCombatTarget = false;
	
	bMovingForward = false;
	bMovingRight = false;

	bESCDown = false;
	bSkillKeyDown = false;

	bHitted = false;

	PerkPoints = 100;

}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();

	SkillNameArray.SetNum(9);
	SkillThumbnailArray.SetNum(9);

	MainPlayerController = Cast<AMainPlayerController>(GetController());

	// �ٸ� ������ �Ѿ �� ����, ����, ü�� ���� �� ������� ����. ��ġ���� �ʿ� x
	FString Map = GetWorld()->GetMapName();
	Map.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	
	UFirstSaveGame* ExistGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	if (ExistGameInstance) {
		if (ExistGameInstance->CharacterStats.LevelName != Map) {
			// ����� ���Ӹ��̶� ���� ������ �� ���̶� �ٸ� ���¶��? ��, �� �̵��� ��츸 �ش� �� ����.
			LoadGameNoSwitch();
			if (MainPlayerController) {
				MainPlayerController->GameModeOnly();
			}
		}
		else {
			// ���� �ٽ� ���� ���. 
			LoadGame(true);
			if (MainPlayerController) {
				MainPlayerController->GameModeOnly();
			}
		}
	}
	else {
		if (MainPlayerController) {
			MainPlayerController->GameModeOnly();
		}
	}

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
			// ���׹̳ʰ� ���� ������ ��
			if (bShiftKeyDown) {
				// �޸��� ���� ���¿���
				if (Stamina - DeltaStamina <= MinSprintStamina) {
					// �����ִ� ���׹̳��� �������� ���϶��
					SetStaminaStatus(EStaminasStatus::ESS_BelowMinimum);
					Stamina -= DeltaStamina;
				}
				else {
					// ���� �̻��̶�� ���׹̳� ���� ������Ʈ �ʿ� x.
					Stamina -= DeltaStamina;
				}
				// �޸����ִ� ���·� ��ȯ���ֱ� 
				if (bMovingForward || bMovingRight) {
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				}
				else {
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
			}
			else {
				// ����Ʈ Ű �� ���¿���
				if (Stamina + DeltaStamina >= MaxStamina) {
					// �߰��Ϸ��� �ִ�ġ �̻��̶��
					Stamina = MaxStamina; // �׳� �ִ�ġ�� �ʱ�ȭ
				}
				else {
					// �ִ�ġ �ƴ϶�� ���׹̳� ȸ�������ֱ�
					Stamina += DeltaStamina;
				}
				// MovementStatus �������ֱ� 
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			break;
		case EStaminasStatus::ESS_BelowMinimum:
			// ���׹̳� ���°� ���������� ����
			if (bShiftKeyDown) {
				// �޸��� ���� ���¿���
				if (Stamina - DeltaStamina <= 0.f) {
					// ���׹̳� <= 0.f �϶�
					SetStaminaStatus(EStaminasStatus::ESS_Exhausted);
					Stamina = 0.f;
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
				else {
					// 0 < ���׹̳� <= BelowMinimun �϶�
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
				// ����Ʈ Ű �� ���¿���
				if (Stamina + DeltaStamina >= MinSprintStamina) {
					// �߰�ȸ���ؼ� >= Min �� �ȴٸ� 
					SetStaminaStatus(EStaminasStatus::ESS_Normal);
					Stamina += DeltaStamina;
				}
				else {
					// �߰� ȸ�� �ص� < min �̶��
					Stamina += DeltaStamina;
				}
				// � ���µ� �ȴ°� ����
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

	// ���� ���� ���ƺ��� ����
	if (bInterpToEnemy && CombatTarget) {
		// �� �ΰ��� ������ �����ϸ� ���� ���� : ���� ���� �ٶ󺸴� ���� 
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);
		
		SetActorRotation(InterpRotation);
	}

	// �� ���� ü�� HUD����
	if (CombatTarget) {
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (MainPlayerController) {
			if (CombatTarget->IsBoss == false) {
				MainPlayerController->EnemyLocation = CombatTargetLocation;
			}
			else {
				MainPlayerController->BossEnemyLocation = CombatTargetLocation;
			}
		}
	}

	// Zoom in, out from Ctrl Key
	if (bZoomingIn) {
		// �� ����� �ִ�
		ZoomFactor += DeltaTime / 0.5f; // 0.5�ʿ� ���� ����
	}
	else {
		ZoomFactor -= DeltaTime / 0.25f; // 0.25�ʾ� ���� �ܾƿ�
	}
	ZoomFactor = FMath::Clamp<float>(ZoomFactor, 0.f, 1.f);
	// ZoomFactor�� ���� ������ ���� ���̿� ī�޶��� �þ� ����
	CameraBoom->TargetArmLength = FMath::Lerp<float>(450.f, 300.f, ZoomFactor);
	// FollowCamera->FieldOfView = FMath::Lerp<float>(90.f, 60.f, ZoomFactor);

	// �κ��丮 �۾�
	CheckForInteractables();
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
	check(PlayerInputComponent); // PlayerInputComponent�� false��� �ڵ� ����

	// ����
	// IE_Pressed : ������ ��
	// IE_Released : ���� ��
	// ACharacter�� �̹� ����߰� �� �ȿ��� Jump�� �����Ǿ� ����.
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMain::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMain::LMBUp);

	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMain::ESCDown);
	PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMain::ESCUp);


	PlayerInputComponent->BindAction("ZoomIn", IE_Pressed, this, &AMain::ZoomIn);
	PlayerInputComponent->BindAction("ZoomIn", IE_Released, this, &AMain::ZoomOut);

	PlayerInputComponent->BindAction("Skill1", IE_Pressed, this, &AMain::Skill1Down);
	PlayerInputComponent->BindAction("Skill1", IE_Released, this, &AMain::SkillKeyUp);

	PlayerInputComponent->BindAction("Skill2", IE_Pressed, this, &AMain::Skill2Down);
	PlayerInputComponent->BindAction("Skill2", IE_Released, this, &AMain::SkillKeyUp);

	/*PlayerInputComponent->BindAction("Skill3", IE_Pressed, this, &AMain::Skill3Down);
	PlayerInputComponent->BindAction("Skill3", IE_Released, this, &AMain::SkillKeyUp);

	PlayerInputComponent->BindAction("Skill4", IE_Pressed, this, &AMain::Skill4Down);
	PlayerInputComponent->BindAction("Skill4", IE_Released, this, &AMain::SkillKeyUp);

	PlayerInputComponent->BindAction("Skill5", IE_Pressed, this, &AMain::Skill5Down);
	PlayerInputComponent->BindAction("Skill5", IE_Released, this, &AMain::SkillKeyUp);

	PlayerInputComponent->BindAction("Skill6", IE_Pressed, this, &AMain::Skill6Down);
	PlayerInputComponent->BindAction("Skill6", IE_Released, this, &AMain::SkillKeyUp);*/
	
	PlayerInputComponent->BindAction("Pickup", IE_Pressed, this, &AMain::SetController_Interact_KeyDown);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMain::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMain::LookUp);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);

}

bool AMain::CanMove(float Value)
{
	if (MainPlayerController) {
		// �Ʒ� 5���� ��� true�� ��� true ������ ����.
		return Value != 0.0f &&
			!bAttacking &&
			MovementStatus != EMovementStatus::EMS_Dead &&
			!MainPlayerController->bPauseMenuVisible &&
			!bHitted;
	}
	return false;
}

void AMain::Turn(float Value)
{
	if (CanMove(Value)) {
		AddControllerYawInput(Value);
	}
}

void AMain::LookUp(float Value)
{
	if (CanMove(Value)) {
		AddControllerPitchInput(Value);
	}
}

void AMain::ZoomIn()
{
	bZoomingIn = true;
}
void AMain::ZoomOut()
{
	bZoomingIn = false;
}


void AMain::MoveForward(float Value) {
	bMovingForward = false; // �����̱� ���� false�� �ʱ�ȭ
	if (CanMove(Value)) {
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		// ī�޶� ��ȯ�Ǹ� �ڿ������� ĳ���͵� �� ���� ��
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

		bMovingForward = true; // �����̴� ���� true�� �ʱ�ȭ
	}
}

void AMain::MoveRight(float Value) {
	bMovingRight = false; // �����̱� ���� false�� �ʱ�ȭ
	if (CanMove(Value)) {
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);

		bMovingRight = true; // �����̴� ���� true�� �ʱ�ȭ
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

	if (MainPlayerController) {
		// PausMenu�� ���� ���¿��� ���ʸ��콺 ������ �����̳� �������� ���ϵ���..
		if (MainPlayerController->bPauseMenuVisible) return;
	}

	if (ActiveOverlappingItem) {
		// ActiveOverlappingItem�� ���ӽ����� Main�������¿��� ������->Item����
		// ActiveOverlappingItem = ���� ������. �׷��ٰ� ����� �������Ǹ� �ٲ�
		// ���� ���ʸ��콺�� �����µ� �����۰� ������ �� ��Ȳ�̶��
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem); // ����ȯ �ϰ�
		if (Weapon) {
			Weapon->Equip(this); // ���� ����! 
			Weapon->SetMainReference(this);
			// �ؿ� �������ϸ� �����ϸ� ActiveOverlappingItem�� ��� �����Ǿ�����
			SetActiveOverlappingItem(nullptr); // null�� ���༭ �ٸ� ���⵵ ������ �� �ְ� ��
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

void AMain::ESCDown()
{
	bESCDown = true;
	if (MainPlayerController) {
		MainPlayerController->TogglePauseMenu();
	}
}

void AMain::ESCUp()
{
	bESCDown = false;
}


FVector AMain::GetSpawnPoint() {
	// GetScaledBoxExtent() : Box�� Extent�� ���ͷ� ��ȯ
	FVector Extent = SkillSpawningBox->GetScaledBoxExtent();
	// GetComponentLocation() : Box�� Origin ��ġ ���ͷ� ��ȯ
	FVector Origin = SkillSpawningBox->GetComponentLocation();
	// RandomPointInBoundingBox : ù ��° ���͸� �������� ����ϰ� �� ��° ���͸� ���� ������ ����Ͽ� ������ ��� ���� ���� ���� ���� ��ȯ�մϴ�.
	FVector Point = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
	return Point;
}

void AMain::DecrementHealth(float Amount) {
	Health -= Amount;
	if (Health - Amount <= 0.f) {
		Die();
	}
}

void AMain::Die() {
	// ���� �̹� ���� ���¶�� �ٷ� �°� �� �ִϸ��̼� �߻����� �ʵ��� ����
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
	if (MainPlayerController) {
		// PausMenu�� ���� ���¿��� Jump�ȵǵ���..
		if (MainPlayerController->bPauseMenuVisible) return;
	}

	if (MovementStatus != EMovementStatus::EMS_Dead && !bSkillKeyDown && !bHitted) {
		ACharacter::Jump();
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

void AMain::SetIsSkillStatus(EIsSkillStatus Status) {
	IsSkillStatus = Status;
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
	// Debug �� ���� 
	/*
	UObject* WorldContextObject : ��� ����?
	FVector const Center : �� ���� ��ġ��?
	float Radius : �� ��������?
	int32 Segments : �� �� ����?
	FLinearColor Color : �� ����?
	float LifeTime : �� �����ϰ� ���ʵ��� ����?
	float Thickness : �� �β���?
	*/

	for (int32 i = 0; i < PickupLocations.Num(); i++) {
		UKismetSystemLibrary::DrawDebugSphere(this, PickupLocations[i] + FVector(0, 0, 100.f), 25.f, 8, FLinearColor::Red, 10.f, .5f);
	}
	// �����̳ʿ��� ���� ������� ���� �ݺ��� ����
	for (FVector Location : PickupLocations) {
		// FVector Location = auto Location
		UKismetSystemLibrary::DrawDebugSphere(this, Location + FVector(0, 0, 100.f), 25.f, 8, FLinearColor::Green, 3.f, 2.f);
	}

}

void AMain::SetEquippedWeapon(AWeapon* WeaponToSet)
{
	
	if (EquippedWeapon) {
		// �̹� �����Ǿ��ִ°� �ִٸ� ������ �� Destroy�ع���
		 EquippedWeapon->Destroy();
	}
	EquippedWeapon = WeaponToSet;
}

void AMain::Attack()
{
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead && !bSkillKeyDown && !bHitted) {
		bAttacking = true;
		SetInterpToEnemy(true);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		// ī�޶� ����ũ �ֱ�
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(CameraShake, 1.f);

		if (AnimInstance && CombatMontage) {
			SetIsSkillStatus(EIsSkillStatus::EMS_Attack);
			int32 Section = FMath::RandRange(1, 2);
			switch (Section)
			{
			case 1:
				AnimInstance->Montage_Play(CombatMontage, 2.2f);
				AnimInstance->Montage_JumpToSection("Attack_1", CombatMontage);
				
				break;
			case 2:
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
	// �ִϸ��̼� �������Ʈ���� ����ϱ� ���� �Լ�
	bAttacking = false;
	SetInterpToEnemy(false);
	if (bLMBDown) {
		// ���ʸ��콺 ��ư ������ �ִ� ���¶�� ��� ����
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
	// ���� ����� �� ã���ִ� �Լ�
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	if (OverlappingActors.Num() == 0) {
		if (MainPlayerController) {
			MainPlayerController->RemoveEnemyHealthBar();
			MainPlayerController->RemoveBossEnemyHealthBar();
		}
		return;
	}

	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]); // ��ģ �迭 �� ���� ù��° �ε����� ��������

	for (auto Enemy : OverlappingActors) {
		// ù��° �ε������� ���鼭, ���� �׾��ִٸ� ���� �ε����� ������ �ʱ�ȭ
		AEnemy* AliveEnemy = Cast<AEnemy>(Enemy);
		if (AliveEnemy->GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead) {
			ClosestEnemy = AliveEnemy;
		}
	}


	if (ClosestEnemy) {
		float MinDistance = (ClosestEnemy->GetActorLocation() - GetActorLocation()).Size();

		for (auto Actor : OverlappingActors) {
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if (Enemy && Enemy->GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead) {
				float DistanceToActor = (Enemy->GetActorLocation() - GetActorLocation()).Size();
				if (MinDistance > DistanceToActor) {
					MinDistance = DistanceToActor;
					ClosestEnemy = Enemy;
				}
			}
		}
		if (MainPlayerController) {
			if(ClosestEnemy->IsBoss == false){
				MainPlayerController->DisplayEnemyHealthBar();
			}
			else {
				// ������ ���
				MainPlayerController->DisplayBossEnemyHealthBar();
			}
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
		CurrentLevel.RemoveFromStart(World->GetMapName());
		// FName�� FString���� ����ȯ ���� but �ݴ�� ���� 
		// �̸� ���ؼ� ���� ���� �����ڸ� ����ϼ�
		FName CurrentLevelName(*CurrentLevel);
		if (CurrentLevelName != LevelName) {
			UGameplayStatics::OpenLevel(World, LevelName);
			LoadGameNoSwitch();
		}
	}
}

void AMain::SaveGame()
{
	UE_LOG(LogTemp, Warning, TEXT("****************************SaveGame Begin****************************"));
	/*
	UFirstSaveGame::StaticClass() :  USaveGame �����Ͱ� ����
	 UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass());
	 CreateSaveGameObject : �����͸� ������ ��� �ִ� �� SaveGame ��ü�� ���� ���� SaveGameToSlot�� �����մϴ�.
	*/
	UFirstSaveGame* SaveGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	SaveGameInstance->CharacterStats.Health = Health;
	SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
	SaveGameInstance->CharacterStats.Stamina = Stamina;
	SaveGameInstance->CharacterStats.MaxStamina = MaxStamina;
	SaveGameInstance->CharacterStats.Coins = Coins;
	SaveGameInstance->CharacterStats.Location = GetActorLocation();
	SaveGameInstance->CharacterStats.Rotation = GetActorRotation();
	SaveGameInstance->CharacterStats.STR = STR;
	SaveGameInstance->CharacterStats.PerkPoints = PerkPoints;
	SaveGameInstance->CharacterStats.SkillNameArray = SkillNameArray;
	SaveGameInstance->CharacterStats.SkillThumbnailArray = SkillThumbnailArray;
	SaveGameInstance->CharacterStats.bIsLevel1Clear = bIsLevel1Clear;
	SaveGameInstance->CharacterStats.bIsLevel2Clear = bIsLevel2Clear;
	SaveGameInstance->CharacterStats.bIsLevel3Clear = bIsLevel3Clear;

	AMainPlayerController* SaveController = Cast<AMainPlayerController>(GetController());
	if (Controller) {
		SaveGameInstance->CharacterStats.CharInventory = SaveController->Inventory; // �κ��丮 ����ü �迭 ���� ����
	}
	
	FString MapName = GetWorld()->GetMapName();
	// LOG�غ��� �� �̸��� UEDPIE_0_SunTemple �̷������� ���� ����Ʈ�� �ٴ°Ŷ� �̰� �����ؾ���
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	SaveGameInstance->CharacterStats.LevelName = MapName;

	if (EquippedWeapon) {
		// Main.cpp���Ͽ��� FirstSaveGame.cpp�� ������ Weapon.cpp���Է� �Ѱ��� 
		SaveGameInstance->CharacterStats.WeaponName = EquippedWeapon->Name;
		UE_LOG(LogTemp, Warning, TEXT("Save WeaponName = %s"), *EquippedWeapon->Name);
	}
	/*
	���� �� �����͸� ��ǻ�Ϳ� �����ϱ� ����
	SaveGameObject�� ������ �÷����� ���� ����/���Ͽ� �����մϴ�.
	@note �̰��� ��� ���Ͻ��� �Ӽ��� ����� ���̸� SaveGame �Ӽ� �÷��״� Ȯ�ε��� �ʽ��ϴ�
	*/
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex);
}

void AMain::LoadGame(bool SetPosition)
{
	UE_LOG(LogTemp, Warning, TEXT("****************************LoadGame Begin****************************"));
	UFirstSaveGame* LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
	Coins = LoadGameInstance->CharacterStats.Coins;
	STR = LoadGameInstance->CharacterStats.STR;
	PerkPoints = LoadGameInstance->CharacterStats.PerkPoints;
	SkillNameArray = LoadGameInstance->CharacterStats.SkillNameArray;
	SkillThumbnailArray = LoadGameInstance->CharacterStats.SkillThumbnailArray;
	bIsLevel1Clear = LoadGameInstance->CharacterStats.bIsLevel1Clear;
	bIsLevel2Clear = LoadGameInstance->CharacterStats.bIsLevel2Clear;
	bIsLevel3Clear = LoadGameInstance->CharacterStats.bIsLevel3Clear;

	AMainPlayerController* LoadController = Cast<AMainPlayerController>(GetController());
	if (LoadController) {
		LoadController->Inventory = LoadGameInstance->CharacterStats.CharInventory;
		LoadController->ReloadInventory();
	}

	if (WeaponStorage) {
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage); // ���⸦ ���� �� ���� ItemStorage���͸� �����ϰ�
		if (Weapons) {
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponName; // SaveGame���� ���� �� ���� ������ ���� ���� �̸��� ��������

			if (Weapons->WeaponMap.Contains(WeaponName)) { // ItemStorage�� WeaponMap�� �ش� �̸�(Key)�� �ش��ϴ� ����BP(Value)�� ������
				// TMap�� Key���� WeaponName�� ���� ��쿡��
				// Weapons->WeaponMap[WeaponName] -> Key : WeaponName  return : �ش�Ǵ� value��
				AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]); // �ش� ����BP ������Ű��
				WeaponToEquip->Equip(this); // ������Ŵ
				UE_LOG(LogTemp, Warning, TEXT("Load WeaponName = %s"), *WeaponToEquip->Name);
			}
			
		}
	}
	/*
	���� ������ ���� �̵��Ѵٴ� ���� Save Load�Ѵٴ°���. 
	�ܼ��ϰ� ���� ������ �Ѿ��
	 �츮�� ��ġ�� ȸ�������� ������ �ʿ�� ����
	*/
	if (SetPosition) {
		// SetPosition�� true��� �� �����̵��� �ƴ϶�� ��.
		SetActorLocation(LoadGameInstance->CharacterStats.Location);
		SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
	 }

	SetMovementStatus(EMovementStatus::EMS_Normal);
	GetMesh()->bPauseAnims = false;
	GetMesh()->bNoSkeletonUpdate = false;

	FString Map = GetWorld()->GetMapName();
	Map.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	if (LoadGameInstance->CharacterStats.LevelName != TEXT("") && LoadGameInstance->CharacterStats.LevelName != Map) {
		FName LevelName(*LoadGameInstance->CharacterStats.LevelName);
		SwitchLevel(LevelName);
	}
}

void AMain::LoadGameNoSwitch()
{
	UE_LOG(LogTemp, Warning, TEXT("****************************LoadGameNoSwitch Begin****************************"));
	UFirstSaveGame* LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
	Coins = LoadGameInstance->CharacterStats.Coins;
	STR = LoadGameInstance->CharacterStats.STR;
	PerkPoints = LoadGameInstance->CharacterStats.PerkPoints;
	SkillNameArray = LoadGameInstance->CharacterStats.SkillNameArray;
	SkillThumbnailArray = LoadGameInstance->CharacterStats.SkillThumbnailArray;
	bIsLevel1Clear = LoadGameInstance->CharacterStats.bIsLevel1Clear;
	bIsLevel2Clear = LoadGameInstance->CharacterStats.bIsLevel2Clear;
	bIsLevel3Clear = LoadGameInstance->CharacterStats.bIsLevel3Clear;

	AMainPlayerController* LoadController = Cast<AMainPlayerController>(GetController());
	if (LoadController) {
		LoadController->Inventory = LoadGameInstance->CharacterStats.CharInventory;
		LoadController->ReloadInventory();
	}

	if (WeaponStorage) {
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapons) {
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;

			if (Weapons->WeaponMap.Contains(WeaponName)) {
				// TMap�� Key���� WeaponName�� ���� ��쿡��
				// Weapons->WeaponMap[WeaponName] -> Key : WeaponName  return : �ش�Ǵ� value��
				AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);
				WeaponToEquip->Equip(this);
				UE_LOG(LogTemp, Warning, TEXT("LoadGameNoSwitch WeaponName = %s"), *WeaponToEquip->Name);
			}

		}
	}

	SetMovementStatus(EMovementStatus::EMS_Normal);
	GetMesh()->bPauseAnims = false;
	GetMesh()->bNoSkeletonUpdate = false;
}


// ====================================== Skill Setting ===============================================
void AMain::SkillKeyUp()
{
	bSkillKeyDown = false;
}

//
void AMain::Skill1Down()
{
	bSkillKeyDown = true;
	if (MainPlayerController) {
		if (MainPlayerController->bPauseMenuVisible) return;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("2"));
	}
	//  && CombatTarget
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead && !bHitted && EquippedWeapon && CombatTarget){
		if (!CombatTarget) {
			UKismetSystemLibrary::PrintString(this, FString("Ÿ��� ���� �����ϴ�!!"));
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("5"));
		}
		bAttacking = true;
		SetInterpToEnemy(true);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance) {
			AnimInstance->Montage_Play(CombatMontage, 2.2f);
			AnimInstance->Montage_JumpToSection(MontageMotionName, CombatMontage);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("3"));
		}
		if (SkillNameArray[0] != "") {
			UE_LOG(LogTemp, Warning, TEXT("SkillNameArray[0] != ' '!! So, Skill1_Operation is operating"));
			Skill1_Operation(1, SkillNameArray[0]);  // Skill1_Operation() �Լ��� �������Ʈ���� ��ų�� Ư¡�� ������ ��ų ��������.  �ִϸ��̼ǵ� ��ų�� ���� �̸� �����ؼ� ���Ը�Ÿ�ֿ� �������ְ���� 
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("SkillNameArray[0] == ' '!! So, Skill1_Operation isn't operating"));
			return;
		} 
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("4"));
	}
}

void AMain::Skill2Down()
{
	bSkillKeyDown = true;
	if (MainPlayerController) {
		if (MainPlayerController->bPauseMenuVisible) return;
	}
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead && !bHitted && EquippedWeapon && CombatTarget) {
		bAttacking = true;
		SetInterpToEnemy(true);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance) {
			AnimInstance->Montage_Play(CombatMontage, 2.2f);
			AnimInstance->Montage_JumpToSection(MontageMotionName, CombatMontage);
		}
		if (SkillNameArray[1] != "") {
			Skill1_Operation(2, SkillNameArray[1]);  // Skill1_Operation() �Լ��� �������Ʈ���� ��ų�� Ư¡�� ������ ��ų ��������.  �ִϸ��̼ǵ� ��ų�� ���� �̸� �����ؼ� ���Ը�Ÿ�ֿ� �������ְ���� 
		}
		else return;
	}
}
void AMain::Skill3Down() {
	bSkillKeyDown = true;
	if (MainPlayerController) {
		if (MainPlayerController->bPauseMenuVisible) return;
	}
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead && !bHitted && EquippedWeapon && CombatTarget) {
		bAttacking = true;
		SetInterpToEnemy(true);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance) {
			AnimInstance->Montage_Play(CombatMontage, 1.8f);
			AnimInstance->Montage_JumpToSection(MontageMotionName, CombatMontage);
		}
		if (SkillNameArray[2] != "") {
			Skill1_Operation(3, SkillNameArray[2]);  // Skill1_Operation() �Լ��� �������Ʈ���� ��ų�� Ư¡�� ������ ��ų ��������.  �ִϸ��̼ǵ� ��ų�� ���� �̸� �����ؼ� ���Ը�Ÿ�ֿ� �������ְ���� 
		}
		else return;
	}
}
void AMain::Skill4Down() {
	bSkillKeyDown = true;
	if (MainPlayerController) {
		if (MainPlayerController->bPauseMenuVisible) return;
	}
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead && !bHitted && EquippedWeapon && CombatTarget) {
		bAttacking = true;
		SetInterpToEnemy(true);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance) {
			AnimInstance->Montage_Play(CombatMontage, 2.2f);
			AnimInstance->Montage_JumpToSection(MontageMotionName, CombatMontage);
		}
		if (SkillNameArray[3] != "") {
			Skill1_Operation(4, SkillNameArray[3]);  // Skill1_Operation() �Լ��� �������Ʈ���� ��ų�� Ư¡�� ������ ��ų ��������.  �ִϸ��̼ǵ� ��ų�� ���� �̸� �����ؼ� ���Ը�Ÿ�ֿ� �������ְ���� 
		}
		else return;
	}
}
void AMain::Skill5Down() {
	bSkillKeyDown = true;
	if (MainPlayerController) {
		if (MainPlayerController->bPauseMenuVisible) return;
	}
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead && !bHitted && EquippedWeapon && CombatTarget) {
		bAttacking = true;
		SetInterpToEnemy(true);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance) {
			AnimInstance->Montage_Play(CombatMontage, 2.2f);
			AnimInstance->Montage_JumpToSection(MontageMotionName, CombatMontage);
		}
		if (SkillNameArray[4] != "") {
			Skill1_Operation(5, SkillNameArray[4]);  // Skill1_Operation() �Լ��� �������Ʈ���� ��ų�� Ư¡�� ������ ��ų ��������.  �ִϸ��̼ǵ� ��ų�� ���� �̸� �����ؼ� ���Ը�Ÿ�ֿ� �������ְ���� 
		}
		else return;
	}
}
void AMain::Skill6Down() {
	bSkillKeyDown = true;
	if (MainPlayerController) {
		if (MainPlayerController->bPauseMenuVisible) return;
	}
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead && !bHitted && EquippedWeapon && CombatTarget) {
		bAttacking = true;
		SetInterpToEnemy(true);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance) {
			AnimInstance->Montage_Play(CombatMontage, 2.2f);
			AnimInstance->Montage_JumpToSection(MontageMotionName, CombatMontage);
		}
		if (SkillNameArray[5] != "") {
			Skill1_Operation(6, SkillNameArray[5]);  // Skill1_Operation() �Լ��� �������Ʈ���� ��ų�� Ư¡�� ������ ��ų ��������.  �ִϸ��̼ǵ� ��ų�� ���� �̸� �����ؼ� ���Ը�Ÿ�ֿ� �������ְ���� 
		}
		else return;
	}
}


void AMain::BeHittedStart()
{
	if (MovementStatus != EMovementStatus::EMS_Dead) {
		SetMovementStatus(EMovementStatus::EMS_Hitted);
		bHitted = true;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance) {
			AnimInstance->Montage_Play(CombatMontage, 1.5f);
			AnimInstance->Montage_JumpToSection(FName("Hitted"), CombatMontage);
		}
		else return;
	}
}

void AMain::BeHittedEnd()
{
	SetMovementStatus(EMovementStatus::EMS_Normal);
	bHitted = false;
	if (bAttacking) bAttacking = false;
}

void AMain::CheckForInteractables()
{
	FHitResult HitResult;

	FVector StartTrace = FollowCamera->GetComponentLocation();
	FVector EndTrace = (FollowCamera->GetForwardVector() * 750.f) + StartTrace;

	FCollisionQueryParams QueryParams; 
	QueryParams.AddIgnoredActor(this); // �� ĳ���� hit ����! 

	AMainPlayerController* MainController = Cast<AMainPlayerController>(GetController());
	if (MainController) {
		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECollisionChannel::ECC_Visibility, QueryParams)) {
			AInventory_Interactable* Interactable = Cast<AInventory_Interactable>(HitResult.GetActor());
			if (Interactable) {
				MainController->CurrentInteractable = Interactable;
				MainController->bTextOnLevel = true;
				MainController->PickupItemLocation = Interactable->GetActorLocation();
				MainController->DisplayPickupText();
				return;
			}
			else {
				MainController->CurrentInteractable = nullptr;
				MainController->bTextOnLevel = false;
				MainController->RemovePickupText();
				return;
			}
		}
		else {
			MainController->CurrentInteractable = nullptr;
			MainController->bTextOnLevel = false;
			MainController->RemovePickupText();
			return;
		}
	}
}
//
void AMain::SetController_Interact_KeyDown()
{
	if (MainPlayerController) {
		MainPlayerController->Interact();
		UE_LOG(LogTemp, Warning, TEXT("Interact() OK"));
	}
	else return;
}