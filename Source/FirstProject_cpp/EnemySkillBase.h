// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySkillBase.generated.h"

UCLASS()
class FIRSTPROJECT_CPP_API AEnemySkillBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySkillBase();

	UPROPERTY()
	class USceneComponent* Scene;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Particle")
	class UParticleSystemComponent* SkillParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Particle")
	class UParticleSystem* HitParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Movement")
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Combat")
	class UBoxComponent* HitCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill | Combat")
	AController* SkillInstigator;


	FORCEINLINE void SetInstigator(AController* Inst) { SkillInstigator = Inst; }

	FTimerHandle DestroyTime;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Sound")
	class USoundCue* SkillOnSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Sound")
	USoundCue* HitSuccessSound;*/

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Combat")
	float DestroyDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Combat")
	float SkillDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Combat")
	float MinDmg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Combat")
	float MaxDmg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Combat")
	bool bFromEnemy;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Disappear();

	UFUNCTION()
	virtual void  HitOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void  HitOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	float SkillDamageRand();

};
