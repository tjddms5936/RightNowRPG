// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Main.h"
#include "FirstSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStats {
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Health;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Stamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxStamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int32 Coins;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FVector Location;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FRotator Rotation;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FString WeaponName;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FString LevelName;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	TArray<FInventoryItem> CharInventory;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int32 STR;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int32 PerkPoints;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	TArray<FName> SkillNameArray;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	TArray<class UTexture2D*>SkillThumbnailArray;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	bool bIsLevel1Clear;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	bool bIsLevel2Clear;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	bool bIsLevel3Clear;
};

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_CPP_API UFirstSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UFirstSaveGame();

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString PlayerName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FCharacterStats CharacterStats;
};
