// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_CPP_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// Similar to beginplay
	virtual void NativeInitializeAnimation() override;

	// 블루프린트에서 함수로써 부를 수 있도록 설정
	UFUNCTION(BlueprintCallable, Category = AnimationProperties)
	void UpdateAnimationProperties();

	// BlueprintReadOnly로 했기때문에 Get은 가능한데 set은 안됨
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	class APawn* Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	class AEnemy* Enemy;
};
