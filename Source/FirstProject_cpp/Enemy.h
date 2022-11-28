// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8
{
	EMS_Idle		 UMETA(DisplayName = "Idle"),
	EMS_MoveToTarget UMETA(DisplayName = "EMS_MoveToTarget"),
	EMS_Attacking	 UMETA(DisplayName = "Attacking"),
	EMS_Dead		 UMETA(DisplayName = "Dead"),
	EMS_Hitted		 UMETA(DisplayName = "Hitted"),

	EMS_MAX			 UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class FIRSTPROJECT_CPP_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// public Class
	// Sets default values for this character's properties
	AEnemy();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EEnemyMovementStatus EnemyMovementStatus;

	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus Status) { EnemyMovementStatus = Status; }
	FORCEINLINE EEnemyMovementStatus GetEnemyMovementStatus() { return EnemyMovementStatus; }

	// This sphere's role is range for chasing character when character come in this sphere
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* AgroSphere;

	// This sphere's role is range for combating with character when character come in this sphere
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* CombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* SetMainCombatTargetCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	USoundCue* AttackSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	class UBoxComponent* CombatCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	UBoxComponent* CombatCollision2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* CombatMontage;

	// CombatSphere안에 들어왔을 때 Enemy가 세팅되는 상대방이 누군지
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	class AMain* CombatTarget;

	// SetMainCombatTargetCollision안에 들어왔을 때 Main을 다른 클래스에서 참조하기 위한 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	AMain* MainReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	FTimerHandle DeathTimer;

	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
	TSubclassOf<class AItemStorage> DropItem;

	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
	TArray<FString> DropItemBpName;

// SkillZone
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<class AEnemySkillBase> Skill_1;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<class AEnemySkillBase> Skill_2;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<class AEnemySkillBase> Skill_3;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<class AEnemySkillBase> Skill_4;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<class AEnemySkillBase> Skill_5;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<class AEnemySkillBase> Skill_6;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<class AEnemySkillBase> Skill_7;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<class AEnemySkillBase> Skill_8;

	UPROPERTY(EditDefaultsOnly, Category = "SKill")
	TSubclassOf<class AEnemySkillBase> DeathEffect;





protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// public Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bOverlappingCombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bAttacking;

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackMinTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackMaxTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DeathDelay;

	// 만약 플레이어가 죽으면 더이상 공격 안하게 하기위한 조건
	bool bHasValidTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool IsBoss;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHitted;

	FVector SpawnSkillLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsMainCombatTargetMe;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	int32 ItemPercent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bIsLevel1Boss;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bIsLevel2Boss;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bIsLevel3Boss;

public:	
	// public Function
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	virtual void AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void SetMainCombatTargetCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void SetMainCombatTargetCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void MoveToTarget(AMain* Target);

	UFUNCTION()
	void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	UFUNCTION(BlueprintCallable)
	void ActivateCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	void Attack();
	
	UFUNCTION(BlueprintCallable)
	void PlayAttackSound();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void ReceiveRadialDamage(float DamageReceived, const class UDamageType* DamageType, FVector Origin, const struct FHitResult& HitInfo, class AController* InstigatedBy, AActor* DamageCauser);

	void Die(AActor* Causer);

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	bool Alive();

	void Disappear();

	UFUNCTION(BlueprintCallable)
	void SetAllCollision_Disabled();

	UFUNCTION(BlueprintCallable)
	void DeathEndEffect();


	void BeHittedStart();

	UFUNCTION(BlueprintCallable)
	void BeHittedEnd();

	void SpawnDropItem();
};
