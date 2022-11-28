// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "Enemy.h"
void UEnemyAnimInstance::NativeInitializeAnimation()
{
	// BeginPlay()�� ���� ����
	if (Pawn == nullptr) {
		Pawn = TryGetPawnOwner();
		if (Pawn) {
			Enemy = Cast<AEnemy>(Pawn);
		}
	}
	else {
		// Pawn ������
		Enemy = Cast<AEnemy>(Pawn);
	}
}

void UEnemyAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr) {
		Pawn = TryGetPawnOwner();
		if (Pawn) {
			Enemy = Cast<AEnemy>(Pawn);
		}
	}

	if (Pawn) {
		FVector Speed = Pawn->GetVelocity(); // Ư�� �����ӿ��� � ���ǵ带 ������ �ִ��� ��ȯ��
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();
	}
}