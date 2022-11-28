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

	// TSubclassOf를 해주면, 블루프린트에서 Spawning 카테고리에서 ACritter로부터 파생된 클래스만 선택 할 수 있음
	// 굉장히 언리얼 엔진의 스페셜한 기능임. 기억해두기! 
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
	이 함수는 블루프린트 로 덮어쓰도록 디자인되었지만, -> 애초에 SpawnOurPawn이라는 이벤트 있음
	기본 내장 구현이 있기도 합니다. 
	메인 함수 이름 끝에 _Implementation 를 붙인
	함수를 추가로 선언한 뒤 코드를 작성하면 됩니다. 
	자동생성 코드는 블루프린트 오버라이드를 찾지 못하면 
	"_Implementation" 메서드를 호출합니다.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spawning")
	void SpawnOurActor(UClass* ToSpawn, const FVector& Location);

};
