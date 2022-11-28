// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Collider.generated.h"

UCLASS()
class FIRSTPROJECT_CPP_API ACollider : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACollider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		class UStaticMeshComponent* MeshComponent_PSE2;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		class USphereComponent* SphereComponent_PSE;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		class USpringArmComponent* SpringarmComponent_PSE;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		class UCameraComponent* CameraComponent_PSE;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
		class UColliderPawnMovementComponent* OurMovementComponent;

	virtual UPawnMovementComponent* GetMovementComponent() const override;

	FORCEINLINE UStaticMeshComponent* GetMeshComponent_PSE() { return MeshComponent_PSE2; }
	FORCEINLINE void SetMeshComponent(UStaticMeshComponent* Mesh) { this->MeshComponent_PSE2 = Mesh; }
	FORCEINLINE USphereComponent* GetSphereComponen_PSE() { return SphereComponent_PSE; }
	FORCEINLINE void SetSphereComponent(USphereComponent* Sphere) { this->SphereComponent_PSE = Sphere; }
	// inline함수보다 FORCELINLINE이 조금 더 빠르다.
	FORCEINLINE USpringArmComponent* GetSpringarmComponent_PSE() { return SpringarmComponent_PSE; }
	FORCEINLINE void SetSpringarmComponent(USpringArmComponent* Springarm) { this->SpringarmComponent_PSE = Springarm; }
	FORCEINLINE UCameraComponent* GetCameraComponent_PSE() { return CameraComponent_PSE; }
	FORCEINLINE void SetCameraComponent(UCameraComponent* Camera) { this->CameraComponent_PSE = Camera; }

private:
	void MoveForward(float input_value);
	void MoveRight(float input_value);

	void PitchCamera(float AxisValue);
	void YawCamera(float AxisValue);

	FVector2D CameraInput;

};
