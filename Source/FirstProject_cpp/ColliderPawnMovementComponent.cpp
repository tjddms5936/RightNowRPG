// Fill out your copyright notice in the Description page of Project Settings.


#include "ColliderPawnMovementComponent.h"

void UColliderPawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime)) {
		return;
	}

	FVector DesiredMovementThisFrame = ConsumeInputVector().GetClampedToMaxSize(1.0f); // 벡터 정규화 하는거임

	if (!DesiredMovementThisFrame.IsNearlyZero()) {
		// 0에 가깝지 않으면? 
		FHitResult Hit;
		SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit);

		// 만약에 우리가 뭔가에 부H혀 그 옆을 미끄러지게하고 싶다면
		if (Hit.IsValidBlockingHit()) {
			//  IsValidBlockingHit() : 어떤 것을 맞았는지 안맞았는지 여부에 따라 true false 리턴
			// 뭔가에 맞았다면?
			SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);
			// 매개변수 설명
			// DesiredMovementThisFrame : 우리가 움직이고 싶은 이동량
			// 1.f - Hit.Time : 그 이후 시간
			// Hit.Normal : 맞은 방향 
			// Hit : 타격 그 자체
		}
	}
}