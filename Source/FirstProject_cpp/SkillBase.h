// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkillBase.generated.h"

UCLASS()
class FIRSTPROJECT_CPP_API ASkillBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkillBase();

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


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SkillInfo")
	class USoundCue* SkillStartSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SkillInfo")
	USoundCue* SkillHittedSound;


	FORCEINLINE void SetInstigator(AController* Inst) { SkillInstigator = Inst; }


	FTimerHandle DestroyTime;
	

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Sound")
	class USoundCue* SkillOnSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Sound")
	USoundCue* HitSuccessSound;*/
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Combat")
	float DestroyDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Combat")
	float SkillDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Combat")
	float MinDmg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Combat")
	float MaxDmg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Combat")
	bool bFromChar;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill | Combat")
	bool bIsWideDmg;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill | Combat")
	bool bIsBuff;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill | Combat")
	int32 SkillCase;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Disappear();

	UFUNCTION()
	virtual void  HitOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void  HitOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	float SkillDamageRand();

	UFUNCTION(BlueprintCallable)
	int32 CheckCase();

	UFUNCTION(BlueprintImplementableEvent)
	void CustomBuffSkill();

};
