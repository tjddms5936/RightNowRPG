// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Main.h"
#include "SkillSystemComponent.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_CPP_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()
// ==============================인벤토리 관련 작업==============================
public:
	// 아이템 제작할 때 인벤토리 변화를 위해서 작성. 엔진 내부에서 작성
	UFUNCTION(BlueprintImplementableEvent)
	void ReloadInventory();

	UFUNCTION(BlueprintCallable, Category = "Utils")
	void Craftitem(FInventoryItem ItemA, FInventoryItem ItemB, AMainPlayerController* Controller);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	void AddItemToInventoryByID(FName ID);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Interact)
	class AInventory_Interactable* CurrentInteractable;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FInventoryItem> Inventory;

	void Interact();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTextOnLevel;
protected:
	// Main에서 이미 override한거라 여기서도 override하면 에러뜸.
	// virtual void SetupInputComponent() override;

// ==============================인벤토리 관련 작업==============================
public:

	/** Reference to the UMG asset in the editor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> HUDOverlayAsset;

	/** Variable to hold the widget after creating it */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* HUDOverlay;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WEnemyHealthBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* EnemyHealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WBossEnemyHealthBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* BossEnemyHealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WPauseMenu;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* PauseMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WPickupText;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* PickupText;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* Status;*/

	bool bEnemyHealthBarVisible;
	bool bBossEnemyHealthBarVisible;

	void DisplayEnemyHealthBar();
	void RemoveEnemyHealthBar();

	void DisplayBossEnemyHealthBar();
	void RemoveBossEnemyHealthBar();

	void DisplayPickupText();
	void RemovePickupText();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bPauseMenuVisible;

	// BlueprintNativeEvent : 함수이름 끝에 _Implementation붙여서 사용
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void DisplayPauseMenu();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void RemovePauseMenu();

	void TogglePauseMenu();


	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	//void DisplayStatus();

	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	//void RemoveStatus();

	//void ToggleStatus();
	//bool bStatusVisible;


	FVector EnemyLocation;
	FVector BossEnemyLocation;


	// 아이템 위치정보를 넘겨 받는지 확인
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	FVector PickupItemLocation;

	void GameModeOnly();

	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;
};
