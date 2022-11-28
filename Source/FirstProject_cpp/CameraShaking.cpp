// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraShaking.h"

UCameraShaking::UCameraShaking()
{
	OscillationDuration = 0.25f; // �� �ʵ��� ����?
	OscillationBlendInTime = 0.1f; // 0~1 ���̰�
	OscillationBlendOutTime = 0.2f; // 0~1 ���̰�

	RotOscillation.Pitch.Amplitude = 5.0f; // ���Ʒ��� ��鸮�� �� 
	RotOscillation.Pitch.Frequency = 50.f; // ���Ʒ��� ���ļ���µ�.. 500�ص� �� ���� ������
	RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	RotOscillation.Yaw.Amplitude = 5.0f; // �翷���� ��鸮�� ��
	RotOscillation.Yaw.Frequency = 50.f; // ���Ʒ��� ���ļ���µ�.. 500�ص� �� ���� ������
	RotOscillation.Yaw.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;

	// Loc Oscillation�� ��ġ �����̶�µ�
	LocOscillation.X.Amplitude = 100.f; // ���� ī�޶� �ƴ϶� ���� ��鸮�� ����
	LocOscillation.X.Frequency = 1.f;
	LocOscillation.X.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
	// Loc�� Zero�� ������. ī�޶� �ϴ� �Ѵ� �����̰�.. ��¼��..

	LocOscillation.Z.Amplitude = 10.f;
	LocOscillation.Z.Frequency = 50.f;
	LocOscillation.Z.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
}

/*
��� ���
1. ī�޶� ����ũ�� �־��ִ� Ÿ�ֿ̹� ����(������� �� ĳ���� ������ �� || �� ĳ���� ���� ��)
2. ���ϴ� Ÿ�̹��� ���Ե� .h���� ���� TSubclassOf<class UCameraShake> CameraShake; �߰� 
3. ������ Ÿ�̹� �Լ��� ����
4. GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(CameraShake, 1.f);
5. PlayCameraShake(CameraShake, 1.f) ���� ���⼭ 1.f�� ���������� �ε� 
*/