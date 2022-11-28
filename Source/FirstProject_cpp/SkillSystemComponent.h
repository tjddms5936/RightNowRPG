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
	FText Name; // ��ų �̸�

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description; // ��ų ����

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Cost; // ��ų ���µ� �ʿ��� ��ų ����Ʈ

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Thumbnail; // ��ų �����

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Tiers; // ��ų �ִ� ����

	// ============== �ش� ����ü�� �ٸ� ������ TMap�� Key�� Value�� Ȱ���ϱ� ���� �۾�
	// Key�ιۿ� �ȵǳ�..? �غ��� �ϴ�
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

	// QuerySill���� ���� ��ȯ�� ���� ������ ����
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
