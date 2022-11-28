// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Main.h"

void UMainAnimInstance::NativeInitializeAnimation() 
{
	if (Pawn == nullptr) {
		// Pawn이 가리키고 있는게 없으면 이 애니매이션 인스턴스의 소유자를 가져오는 유용한 처리
		Pawn = TryGetPawnOwner(); 
		if (Pawn) {
			Main = Cast<AMain>(Pawn);
		}
	}
	else {
		Main = Cast<AMain>(Pawn);
	}
}

// 스피드를 업데이트 해줄 수 있는 Custom 함수 
// 블루프린트에서 EveryFrame마다 이 함수 호출하도록 노드 연결시켜놨음
void UMainAnimInstance::UpdateAnimationProperties() 
{
	if (Pawn == nullptr) {
		Pawn = TryGetPawnOwner();
		if (Pawn) {
			Main = Cast<AMain>(Pawn);
		}
	}
	if (Pawn) {
		FVector Speed = Pawn->GetVelocity(); // 특정 프레임에서 어떤 스피드를 가지고 있는지 반환함
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();

		// 폰이 공중에 있는 상태라면 true 아니면 false로 설정됨
		bIsInAir = Pawn->GetMovementComponent()->IsFalling();

		if (Main == nullptr) {
			Main = Cast<AMain>(Pawn);
		}
	}
}