// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillSystemComponent.h"

// Sets default values for this component's properties
USkillSystemComponent::USkillSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USkillSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USkillSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


bool USkillSystemComponent::AddSkill(FString RowName) {
	if (SkillUnlocks.Contains(RowName)) {
		int32 AddElements = SkillUnlocks[RowName];
		SkillUnlocks.Add(RowName, AddElements + 1);
		return true;
	}
	else {
		SkillUnlocks.Add(RowName, 1);
		return true;
	}
	return false;
}

bool USkillSystemComponent::QuerySkill(FString RowName, int32& TierLevelRef) {
	if (SkillUnlocks.Contains(RowName)) {
		TierLevelRef = SkillUnlocks[RowName];
		return true;
	}
	else {
		return false;
	}
}
