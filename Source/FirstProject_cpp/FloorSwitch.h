// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorSwitch.generated.h"

UCLASS()
class FIRSTPROJECT_CPP_API AFloorSwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloorSwitch();

	/** Overlab volume for functionality to be triggered */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	class UBoxComponent* TriggerBox;

	/** Switch for the character to step on */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	class UStaticMeshComponent* FloorSwitch;

	/** Door to move when the floor switch is stepped on */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	UStaticMeshComponent* Door;
	
	/** Initial location for the door */
	UPROPERTY(BlueprintReadWrite, Category = "Floor Switch")
	FVector InitialDoorLocation;

	/** Initial location for the floor switch */
	UPROPERTY(BlueprintReadWrite, Category = "Floor Switch")
	FVector InitialSwitchLocation;

	FTimerHandle SwitchHandle;

	UPROPERTY(BlueprintReadWrite, Category = "Floor Switch")
	float SwitchTime;

	bool bCharacterOnSwitch;

	void CloseDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// BlueprintImplementableEvent : C++로 구현 할 필요가 없슴. 즉, 블루프린트에서 기능을 구현 가능.
	// 이걸 엔진에서 호출시켜보면 Custom Event라는 것을 알 수 있다.
	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
	void RaiseDoor();

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
	void LowerDoor();

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
	void RaiseFloorSwitch();

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
	void LowerFloorSwitch();

	UFUNCTION(BlueprintCallable, Category = "Floor Switch")
	void UpdateDoorLocation(float Z);

	UFUNCTION(BlueprintCallable, Category = "Floor Switch")
	void UpdateFloorSwitchLocation(float Z);
};
