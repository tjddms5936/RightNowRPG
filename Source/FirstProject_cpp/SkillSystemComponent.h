// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "SkillSystemComponent.generated.h"


USTRUCT(BlueprintType)
struct FSkillDataStructure : public FTableRowBase
{
	GENERATED_BODY()
public:
	FSkillDataStructure() {
		Name = FText::FromString("Skill Name");
		Description = FText::FromString("Skill Description");
		Cost = 1;
		Tiers = 1;
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name; // 스킬 이름

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description; // 스킬 설명

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Cost; // 스킬 배우는데 필요한 스킬 포인트

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Thumbnail; // 스킬 썸네일

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Tiers; // 스킬 최대 레벨

	// ============== 해당 구조체를 다른 곳에서 TMap의 Key나 Value로 활용하기 위한 작업
	// Key로밖에 안되나..? 해보자 일단
	/*int32 SkillKey;

	bool operator== (const FSkillDataStructure& Other) {
		return SkillKey == Other.SkillKey;
	}
	
	friend uint32 GetTypeHash(const FSkillDataStructure& Other) {
		return GetTypeHash(Other.SkillKey);
	}*/
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIRSTPROJECT_CPP_API USkillSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USkillSystemComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Data")
	TMap<FString, int32> SkillUnlocks;

	// QuerySill에서 다중 반환을 위해 참조할 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save Data")
	int32 TierLevel;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	bool AddSkill(FString RowName);
	
	UFUNCTION(BlueprintCallable)
	bool QuerySkill(FString RowName, int32& TierLevelRef);
		
};
