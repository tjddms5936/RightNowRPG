// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_CPP_API UMainAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:

	// Similar to beginplay
	virtual void NativeInitializeAnimation() override;

	// �������Ʈ���� �Լ��ν� �θ� �� �ֵ��� ����
	UFUNCTION(BlueprintCallable, Category = AnimationProperties)
	void UpdateAnimationProperties();

	// BlueprintReadOnly�� �߱⶧���� Get�� �����ѵ� set�� �ȵ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	class APawn* Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	class AMain* Main;

};
