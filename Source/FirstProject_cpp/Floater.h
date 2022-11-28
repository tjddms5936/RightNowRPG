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

	// �� ���� �� ���� ������ ����ϰ� ���̴� ���� ����� ��! 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ActorComponents")
		UStaticMeshComponent* StaticMesh_PSE;
	// �̰� �������ν� �������� ���� ������ ������ Static Mesh�� �������� �����Ǿ� ����

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = " Floater Variables")
		// ReadWrite Means : We can use the get, set 
		// EditInstanceOnly Mean : This Actor location is able to only in one instance.
		FVector Initialization_PSE;
	// FVector Initialization_PSE = FVector(0.0f); // Fvector�� ���� �����ͷ� ������� �ʴ´�.
	// �̷��� �������μ� �츮�� Floater ���͸� ������ ������ �޽��� �ʱ���ġ���� �츮 ����� ���� ����

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Floater Variables")
		// VIsibleInstanceOnly Mean : We can only see in instance not editable
		FVector PlacedLocation_PSE; // PlacedLocation�� �츮�� Actor�� ���� ���� �� �� ���� ��ġ��

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Floater Variables")
		FVector WorldOrigin_PSE;
	// ȿ�� : �������Ʈ���� ������ ���� + �ν��Ͻ�����(�������Ʈ���ͼ���) �� �� ����. ��, ����Ʈ���� ǥ�õ�

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floater Variables")
		FVector InitialDirection_PSE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floater Variables")
		bool bShoultFloat_PSE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Floater Variables")
		// EditDefaultsOnly Mean : This variable is goint to be using edit defaults only.
		bool bInitializeFloaterLocation_PSE; // ������ ���� b�� cpp���� ��������� bool �ǹ�

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
