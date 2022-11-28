// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class FIRSTPROJECT_CPP_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	class UBoxComponent* SpawningBox;

	// TSubclassOf�� ���ָ�, �������Ʈ���� Spawning ī�װ����� ACritter�κ��� �Ļ��� Ŭ������ ���� �� �� ����
	// ������ �𸮾� ������ ������� �����. ����صα�! 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> Actor_1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> Actor_2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> Actor_3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> Actor_4;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	TArray<TSubclassOf<AActor>> SpawnArray;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Pure Function that returning random point
	UFUNCTION(BlueprintPure, Category = "Spawning")
	FVector GetSpawnPoint();

	UFUNCTION(BlueprintPure, Category = "Spawning")
	TSubclassOf<AActor> GetSpawnActor();

	/**
	* BlueprintNativeEvent
	�� �Լ��� �������Ʈ �� ������� �����εǾ�����, -> ���ʿ� SpawnOurPawn�̶�� �̺�Ʈ ����
	�⺻ ���� ������ �ֱ⵵ �մϴ�. 
	���� �Լ� �̸� ���� _Implementation �� ����
	�Լ��� �߰��� ������ �� �ڵ带 �ۼ��ϸ� �˴ϴ�. 
	�ڵ����� �ڵ�� �������Ʈ �������̵带 ã�� ���ϸ� 
	"_Implementation" �޼��带 ȣ���մϴ�.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spawning")
	void SpawnOurActor(UClass* ToSpawn, const FVector& Location);

};
