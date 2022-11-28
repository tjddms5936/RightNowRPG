// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "Inventory_Pickup.h"
#include "Main.generated.h"

UENUM(BlueprintType)
enum class EIsSkillStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Attack UMETA(DisplayName = "Attack"),
	EMS_Skill UMETA(DisplayName = "Skill"),

	EMS_Max UMETA(DisplayNmae = "DefaultMax")
};

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Dead UMETA(DisplayName = "Dead"),
	EMS_Hitted UMETA(DisplayName = "Hitted"),

	EMS_MAX UMETA(DisplayName = "DefaulMAX")
};

UENUM(BlueprintType)
enum class EStaminasStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_MAX UMETA(DisplayName = "Default MAX")
};


USTRUCT(BlueprintType)
struct FCraftingInfo : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ComponentID; // 조합하려는 ItemB의 ID값 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ProductID; // 조합하고서 탄생하는 ItemC의 ID값

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDestroyItemA; // 조합하고서 ItemA가 파괴되는지 여부

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDestroyItemB; // 조합하고서 ItemB가 파괴되는지 여부
};


USTRUCT(BlueprintType)
struct FInventoryItem : public FTableRowBase // 데이터 테이블 사용하기 위해 상속받음
{
	GENERATED_BODY()
public:
	FInventoryItem() {
		Name = FText::FromString("Item");
		Action = FText::FromString("Use");
		Description = FText::FromString("Please enter a description for this item");
		Value = 10; 
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AInventory_Pickup> ItemPickup;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Thumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCraftingInfo> CraftCombinations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanBeUsed; // 물약같은건 ok, 책같은건 x

	// 인벤토리에서 아이템 삭제하기 위한 작업에 필요  : 진짜 필요한건가?,, 마지막에 주석처리해보자
	bool operator==(const FInventoryItem& Item) const
	{
		if (ItemID == Item.ItemID) return true;
		else return false;
	}
};
//******************************************************************************************
//											public Class
//******************************************************************************************
UCLASS()
class FIRSTPROJECT_CPP_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
	TSubclassOf<class AItemStorage> WeaponStorage;

	FORCEINLINE void SetStaminaStatus(EStaminasStatus Status) { this->StaminaStatus = Status; }

	/** Camera boom positioning the camera behind the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* HitSound;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Items)
	class AWeapon* EquippedWeapon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Data")
	TArray<FName>SkillNameArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Data")
	TArray<class UTexture2D*>SkillThumbnailArray;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill")
	TMap<FName, TSubclassOf<class ASkillBase>>Skill;


	// 메테오 연출을 위해 만든거
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	class UBoxComponent* SkillSpawningBox;

	/**
	When you overlapped with item, you can choice whether to equip it or not.
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
	class AItem* ActiveOverlappingItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy* CombatTarget;

	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AMainPlayerController* MainPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<class AEnemy> EnemyFilter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<class UCameraShake> CameraShake;


//******************************************************************************************
// 										 public Valuables
//******************************************************************************************
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminasStatus StaminaStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EIsSkillStatus IsSkillStatus;
	// 기본적으로 비어있는데, 우리는 픽업할 때 위치를 저장하려고 함 -> Pickup.h에서 추가작업
	TArray<FVector> PickupLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;

	/** Base turn rates to scale turning functions for the camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Stats")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Stats")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Stats")
	float MaxStamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Stats")
	float Stamina; // 마력으로 상정하자

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Stats")
	int32 Coins;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 STR;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 DEF;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Running")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Running")
	float SprintingSpeed;

	bool bShiftKeyDown;

	bool IsMoveKeyDown;

	bool bLMBDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	float InterpSpeed;

	bool bInterpToEnemy;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasCombatTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	FVector CombatTargetLocation;

	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }

	bool bMovingForward;
	bool bMovingRight;

	bool bESCDown;
	// bool bPKeyDown;

	bool bSkillKeyDown;

	float ZoomFactor;
	bool bZoomingIn;

	bool bHitted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	int32 PerkPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	FName MontageMotionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	bool bIsLevel1Clear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	bool bIsLevel2Clear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	bool bIsLevel3Clear;
//******************************************************************************************
//										public Protected
//******************************************************************************************
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



//******************************************************************************************
//										public Function
//******************************************************************************************
public:	
	// Called every frame

	UFUNCTION(BlueprintCallable)
	void ShowPickupLocation();

	/** Set movement status and running speed */
	void SetMovementStatus(EMovementStatus Status);

	UFUNCTION(BlueprintCallable)
	void SetIsSkillStatus(EIsSkillStatus Status);

	/** Pressed down to enable sprinting */
	void ShiftKeyDown();

	/** Released to stop sprinting */
	void ShiftKeyUp();

	void DecrementHealth(float Amount);

	void Die();

	UFUNCTION(BlueprintCallable)
	void IncrementCoins(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amount);

	UFUNCTION(BlueprintCallable)
	void IncrementStamina(float Amount);

	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for forwards/backwards input */
	void MoveForward(float Value);
	
	/** Called for side to side input */
	void MoveRight(float Value);

	/** Called for Yaw rotation */
	void Turn(float Value);

	/** Called for Pitch rotation */
	void LookUp(float Value);

	/** Called for Zoom in  */
	void ZoomIn();
	/** Called for Zoom out */
	void ZoomOut();
	



	/** Called via input to turn at a given rate 
	* @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);
	/** Called via input to loot up/down at a given rate
	* @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	void LMBDown();
	void LMBUp();

	void ESCDown();
	void ESCUp();

	void PKeyDown();
	void PKeyUp();

	void SkillKeyUp();
	void Skill1Down();
	void Skill2Down();
	void Skill3Down();
	void Skill4Down();
	void Skill5Down();
	void Skill6Down();

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void SetEquippedWeapon(AWeapon* WeaponToSet);

	UFUNCTION(BlueprintCallable)
	AWeapon* GetEquippedWeapon() { return EquippedWeapon; }

	FORCEINLINE void SetActiveOverlappingItem(AItem* ItemToSet) { ActiveOverlappingItem = ItemToSet; }
	
	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();

	UFUNCTION(BlueprintCallable)
	void SetInterpToEnemy(bool Interp);

	FRotator GetLookAtRotationYaw(FVector Target);

	// APawn의 TakeDamge() 함수 재정의
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	virtual void Jump() override;

	UFUNCTION(BlueprintCallable)
	void UpdateCombatTarget();

	void SwitchLevel(FName LevelName);

	UFUNCTION(BlueprintCallable)
	void SaveGame();

	// SetPosition is boolien whether you just moving another location or jumping another Level
	UFUNCTION(BlueprintCallable)
	void LoadGame(bool SetPosition);

	// Using function when jumping another level.
	void LoadGameNoSwitch();

	bool CanMove(float Value);

	void EquipSocket();

	FVector GetSpawnPoint();

	void BeHittedStart();

	UFUNCTION(BlueprintCallable)
	void BeHittedEnd();


	// 필드상에서 아이템이랑 일정거리 이내로 들어온 상태인지 확인.
	void CheckForInteractables();

	void SetController_Interact_KeyDown();

	UFUNCTION(BlueprintImplementableEvent)
	void Skill1_Operation(int32 KeyNum, FName SkillName);
	
	
};
