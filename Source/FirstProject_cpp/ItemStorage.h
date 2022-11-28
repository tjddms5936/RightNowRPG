// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemStorage.generated.h"

UCLASS()
class FIRSTPROJECT_CPP_API AItemStorage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemStorage();

	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
	TMap<FString, TSubclassOf<class AWeapon>> WeaponMap;

	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
	TMap<FString, TSubclassOf<class AInventory_Pickup>> DropItemMap;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame -> �츰 �ʿ����
	// virtual void Tick(float DeltaTime) override;

	/*
	 Map ����ؼ� �迭�� ���� ��� ���
	 Map�� key�� ������ ������ key�� � ��ü Ÿ���� �����Ͷ� �� �� �ִ�.
	 TMap�� TArray�� ����Ѱ���.
	
	 Key : ù ��° ���ø� �Ű� ���� �� ù ��°�� F ���ڿ��̰�
	 Value : �� ��° ���ø� �Ű� ������ AWeapon�� T ���� Ŭ���� �Դϴ�.
	 ��, WeaponMap�� Key�� Value�� ���� �迭��.
	*/

	/*
	EditDefaultsOnly
	�� �Ӽ��� �Ӽ� â���� ������ �� ������ ��ŰŸ�Կ����� ������ �� ������ ��Ÿ���ϴ�.
	�� �����ڴ� "���̴�" �����ڿ� ȣȯ���� �ʽ��ϴ�.
	*/
	
	/* WeaponMap.Add("Key", MyWeapon)�� ����Ǿ� �ְ�
	WeaponMap["Key"] �� �ϸ� MyWeapon�� return�ȴ�. 
	�ٵ� �츮�� �̷��� �ϳ��ϳ� .Add�� �ϴ°� �ƴ϶� 
	EditDefaulsOnly�� �����������ν� �������Ʈ ������ �迭�� �߰��� �� �ִ�.
	*/

};
