// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Main.h"

void UMainAnimInstance::NativeInitializeAnimation() 
{
	if (Pawn == nullptr) {
		// Pawn�� ����Ű�� �ִ°� ������ �� �ִϸ��̼� �ν��Ͻ��� �����ڸ� �������� ������ ó��
		Pawn = TryGetPawnOwner(); 
		if (Pawn) {
			Main = Cast<AMain>(Pawn);
		}
	}
	else {
		Main = Cast<AMain>(Pawn);
	}
}

// ���ǵ带 ������Ʈ ���� �� �ִ� Custom �Լ� 
// �������Ʈ���� EveryFrame���� �� �Լ� ȣ���ϵ��� ��� ������ѳ���
void UMainAnimInstance::UpdateAnimationProperties() 
{
	if (Pawn == nullptr) {
		Pawn = TryGetPawnOwner();
		if (Pawn) {
			Main = Cast<AMain>(Pawn);
		}
	}
	if (Pawn) {
		FVector Speed = Pawn->GetVelocity(); // Ư�� �����ӿ��� � ���ǵ带 ������ �ִ��� ��ȯ��
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();

		// ���� ���߿� �ִ� ���¶�� true �ƴϸ� false�� ������
		bIsInAir = Pawn->GetMovementComponent()->IsFalling();

		if (Main == nullptr) {
			Main = Cast<AMain>(Pawn);
		}
	}
}