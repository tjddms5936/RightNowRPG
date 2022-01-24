// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"


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
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();

	 
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
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
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
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
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
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
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
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
	if (Controller != nullptr && Value != 0.0f && (!bAttacking)) {
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		// 카메라 전환되면 자연스럽게 캐릭터도 그 방향 봄
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMain::MoveRight(float Value) {
	if (Controller != nullptr && Value != 0.0f && (!bAttacking)) {
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
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
	if (ActiveOverlappingItem) {
		// ActiveOverlappingItem은 게임시작후 Main누른상태에서 디테일->Item보면
		// ActiveOverlappingItem = 없음 상태임. 그러다가 무기랑 오버렙되면 바뀜
		// 만약 왼쪽마우스를 눌렀는데 아이템과 오버랩 된 상황이라면
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem); // 형변환 하고
		if (Weapon) {
			UE_LOG(LogTemp, Warning, TEXT("Weapon Equip!"));
			Weapon->Equip(this); // 무기 장착! 
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

}

void AMain::IncrementCoins(int32 Amount) {
	this->Coins += Amount;
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
	if (!bAttacking) {
		bAttacking = true;

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
	bAttacking = false;
	if (bLMBDown) {
		// 왼쪽마우스 버튼 누르고 있는 상태라면 계속 공격
		Attack();
	}
}