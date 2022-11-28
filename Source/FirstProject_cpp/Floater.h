// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Floater.generated.h"

UCLASS()
class FIRSTPROJECT_CPP_API AFloater : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloater();

	// 이 다음 두 줄은 앞으로 요긴하게 쓰이니 구조 기억할 것! 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ActorComponents")
		UStaticMeshComponent* StaticMesh_PSE;
	// 이걸 해줌으로써 엔진에서 액터 디테일 봤을때 Static Mesh가 정적으로 생성되어 있음

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = " Floater Variables")
		// ReadWrite Means : We can use the get, set 
		// EditInstanceOnly Mean : This Actor location is able to only in one instance.
		FVector Initialization_PSE;
	// FVector Initialization_PSE = FVector(0.0f); // Fvector는 보통 포인터로 선언되지 않는다.
	// 이렇게 해줌으로서 우리는 Floater 액터를 생성할 때마다 메쉬도 초기위치값도 우리 맘대로 설정 가능

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Floater Variables")
		// VIsibleInstanceOnly Mean : We can only see in instance not editable
		FVector PlacedLocation_PSE; // PlacedLocation은 우리가 Actor를 끌고 왔을 때 그 곳의 위치임

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Floater Variables")
		FVector WorldOrigin_PSE;
	// 효과 : 블루프린트에서 볼수만 있음 + 인스턴스에서(블루프린트나와서는) 볼 수 없음. 즉, 디폴트에만 표시됨

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floater Variables")
		FVector InitialDirection_PSE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floater Variables")
		bool bShoultFloat_PSE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Floater Variables")
		// EditDefaultsOnly Mean : This variable is goint to be using edit defaults only.
		bool bInitializeFloaterLocation_PSE; // 변수명 앞의 b는 cpp에서 통상적으로 bool 의미

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Floater Variables");
	FVector InitialForce;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Floater Variables");
	FVector InitialTorque;

	// Amplitude : How fast amplitude -> A * FMath::Sin(B * RunningTime - C) + D
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floater Variables")
		float A;

	// Period : How fast RunningTime -> A * FMath::Sin(B * RunningTime - C) + D
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floater Variables")
		float B;

	// Phase Shift : A * FMath::Sin(B * RunningTime - C) + D
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floater Variables")
		float C;

	// Vertical Shift : A * FMath::Sin(B * RunningTime - C) + D
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floater Variables")
		float D;

private:
	float RunningTime;

	float BaseZLocation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
