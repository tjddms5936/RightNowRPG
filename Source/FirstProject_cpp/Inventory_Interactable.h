// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inventory_Interactable.generated.h"

UCLASS()
class FIRSTPROJECT_CPP_API AInventory_Interactable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInventory_Interactable();

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	//class USphereComponent* SphereComponent;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*UFUNCTION()
	virtual void SphereComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void SphereComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);*/
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// �� Ŭ������ ��ӹ޴� Inventory_Pickup���� �� �Լ��� override�ؼ� �ʵ�󿡼� �������� ������� �ϰ�, �κ��丮�� �־���(Controller ����ȯ �̿�)
	UFUNCTION(BlueprintImplementableEvent)
	void Interact(class AMainPlayerController* Controller);

	UPROPERTY(EditDefaultsOnly)
	FString Name;

	UPROPERTY(EditDefaultsOnly)
	FString Action;

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	FString GetUseText() const { return FString::Printf(TEXT("%s : Press E to %s"), *Name, *Action); }

};
