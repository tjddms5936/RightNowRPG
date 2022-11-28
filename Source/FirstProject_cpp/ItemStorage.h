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
	// Called every frame -> 우린 필요없음
	// virtual void Tick(float DeltaTime) override;

	/*
	 Map 사용해서 배열과 같은 방식 사용
	 Map은 key를 가지고 있으며 key는 어떤 객체 타입의 데이터라도 될 수 있다.
	 TMap은 TArray와 비슷한것임.
	
	 Key : 첫 번째 템플릿 매개 변수 중 첫 번째는 F 문자열이고
	 Value : 두 번째 템플릿 매개 변수는 AWeapon의 T 하위 클래스 입니다.
	 즉, WeaponMap은 Key와 Value를 가진 배열임.
	*/

	/*
	EditDefaultsOnly
	이 속성은 속성 창에서 편집할 수 있지만 아키타입에서만 편집할 수 있음을 나타냅니다.
	이 지정자는 "보이는" 지정자와 호환되지 않습니다.
	*/
	
	/* WeaponMap.Add("Key", MyWeapon)로 연결되어 있고
	WeaponMap["Key"] 를 하면 MyWeapon이 return된다. 
	근데 우리는 이렇게 하나하나 .Add를 하는게 아니라 
	EditDefaulsOnly를 설정해줌으로써 블루프린트 내에서 배열을 추가할 수 있다.
	*/

};
