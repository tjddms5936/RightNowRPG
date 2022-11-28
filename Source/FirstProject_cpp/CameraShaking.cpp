// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraShaking.h"

UCameraShaking::UCameraShaking()
{
	OscillationDuration = 0.25f; // 몇 초동안 진동?
	OscillationBlendInTime = 0.1f; // 0~1 사이값
	OscillationBlendOutTime = 0.2f; // 0~1 사이값

	RotOscillation.Pitch.Amplitude = 5.0f; // 위아래로 흔들리는 값 
	RotOscillation.Pitch.Frequency = 50.f; // 위아래의 주파수라는데.. 500해도 별 차이 못느낌
	RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	RotOscillation.Yaw.Amplitude = 5.0f; // 양옆으로 흔들리는 값
	RotOscillation.Yaw.Frequency = 50.f; // 위아래의 주파수라는데.. 500해도 별 차이 못느낌
	RotOscillation.Yaw.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;

	// Loc Oscillation은 위치 진동이라는데
	LocOscillation.X.Amplitude = 100.f; // 뭔가 카메라가 아니라 땅이 흔들리는 느낌
	LocOscillation.X.Frequency = 1.f;
	LocOscillation.X.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
	// Loc만 Zero인 이유는. 카메라가 일단 한대 쓸것이고.. 어쩌구..

	LocOscillation.Z.Amplitude = 10.f;
	LocOscillation.Z.Frequency = 50.f;
	LocOscillation.Z.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
}

/*
사용 방법
1. 카메라 쉐이크를 넣어주는 타이밍에 가서(예를들어 내 캐릭이 공격할 때 || 내 캐릭이 맞을 때)
2. 원하는 타이밍이 포함된 .h파일 가서 TSubclassOf<class UCameraShake> CameraShake; 추가 
3. 적당한 타이밍 함수에 가서
4. GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(CameraShake, 1.f);
5. PlayCameraShake(CameraShake, 1.f) 에서 여기서 1.f는 진동스케일 인듯 
*/