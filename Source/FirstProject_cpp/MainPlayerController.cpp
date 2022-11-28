// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "Inventory_Interactable.h"
#include "FirstProject_cppGameModeBase.h"
#include "Blueprint/UserWidget.h"


void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();
	/* 위젯 설정에 있어 반드시 엔진에 들어가서 관련 컨트롤러 들어가서
	1. 위젯 관련 디테일에서
	2. 위젯 설정해주기
	*/

	if (HUDOverlayAsset) {
		UE_LOG(LogTemp, Warning, TEXT("HUDOverlayAsset OK"));
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);

		// 이 위젯을 뷰포트에 추가 하기 위한 작업
	}
	HUDOverlay->AddToViewport();
	HUDOverlay->SetVisibility(ESlateVisibility::Visible);


	if (WEnemyHealthBar) {
		UE_LOG(LogTemp, Warning, TEXT("WEnemyHealthBar OK"));
		EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
	}
	if (EnemyHealthBar){
		EnemyHealthBar->AddToViewport();
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
	FVector2D Alignment(0.f, 0.f);
	EnemyHealthBar->SetAlignmentInViewport(Alignment);


	if (WBossEnemyHealthBar) {
		UE_LOG(LogTemp, Warning, TEXT("WBossEnemyHealthBar OK"));
		BossEnemyHealthBar = CreateWidget<UUserWidget>(this, WBossEnemyHealthBar);
	}
	if (BossEnemyHealthBar) {
		BossEnemyHealthBar->AddToViewport();
		BossEnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
	BossEnemyHealthBar->SetAlignmentInViewport(Alignment);



	if (WPickupText) {
		UE_LOG(LogTemp, Warning, TEXT("WPickupText OK"));
		PickupText = CreateWidget<UUserWidget>(this, WPickupText);
	}
	if (PickupText) {
		PickupText->AddToViewport();
		PickupText->SetVisibility(ESlateVisibility::Hidden);
	}
	PickupText->SetAlignmentInViewport(Alignment);



	if (WPauseMenu) {
		UE_LOG(LogTemp, Warning, TEXT("WPauseMenu OK"));
		PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);
	}
	if (PauseMenu) {
		PauseMenu->AddToViewport();
		PauseMenu->SetVisibility(ESlateVisibility::Hidden);
	}

	/*if (WStatus) {
		UE_LOG(LogTemp, Warning, TEXT("WStatus OK"));
		Status = CreateWidget<UUserWidget>(this, WStatus);
	}
	if (Status) {
		Status->AddToViewport();
		Status->SetVisibility(ESlateVisibility::Hidden);
	}*/
}

void AMainPlayerController::DisplayEnemyHealthBar()
{
	if (EnemyHealthBar) {
		bEnemyHealthBarVisible = true;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveEnemyHealthBar()
{
	if (EnemyHealthBar) {
		bEnemyHealthBarVisible = false;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::DisplayBossEnemyHealthBar()
{
	if (BossEnemyHealthBar) {
		bBossEnemyHealthBarVisible = true;
		BossEnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveBossEnemyHealthBar()
{
	if (BossEnemyHealthBar) {
		bBossEnemyHealthBarVisible = false;
		BossEnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::DisplayPickupText()
{
	if (PickupText) {
		PickupText->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemovePickupText()
{
	if (PickupText) {
		PickupText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyHealthBar) {
		FVector2D PositionInViewport; // 위젯 위치 설정
		// World Space 3D 위치를 2D Screen Space 위치로 변환합니다.
		// 월드 좌표가 화면에 성공적으로 투영된 경우 true 리턴함
		ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport);
		PositionInViewport.Y -= 85.f;
		PositionInViewport.X -= 120.f;


		FVector2D SizeInViewport(350.f, 25.f); // 위제 사이즈 설정

		EnemyHealthBar->SetPositionInViewport(PositionInViewport);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
	}

	if (BossEnemyHealthBar) {
		FVector2D PositionInViewport;
		ProjectWorldLocationToScreen(BossEnemyLocation, PositionInViewport); // 3D위치값을 2D로 바꿔주는 작업. 
		PositionInViewport.Y -= 85;
		PositionInViewport.X -= 260.f;

		FVector2D SizeInViewport(1000.f, 25.f);

		BossEnemyHealthBar->SetPositionInViewport(PositionInViewport);
		BossEnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
	}

	if (PickupText) {
		FVector2D PositionInViewport;
		if (CurrentInteractable) {
			ProjectWorldLocationToScreen(CurrentInteractable->GetActorLocation(), PositionInViewport);
			PositionInViewport.Y -= 60;

			FVector2D SizeInViewport(350.f, 25.f);
			PickupText->SetPositionInViewport(PositionInViewport);
			PickupText->SetDesiredSizeInViewport(SizeInViewport);
		}
	}
}

void AMainPlayerController::DisplayPauseMenu_Implementation()
{
	if (PauseMenu) {
		bPauseMenuVisible = true;
		PauseMenu->SetVisibility(ESlateVisibility::Visible);

		FInputModeGameAndUI InputModeGameAndUI;
		SetInputMode(InputModeGameAndUI);
		bShowMouseCursor = true;
	}
}

void AMainPlayerController::RemovePauseMenu_Implementation()
{
	if (PauseMenu) {
		GameModeOnly();

		bShowMouseCursor = false;

		bPauseMenuVisible = false;
		// PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		// 위의 설정은 블루프린트에서 딜레이 주고 해줌으로써 애니메이션 정상작동해줌

	}
}

void AMainPlayerController::TogglePauseMenu()
{
	if (bPauseMenuVisible) {
		// true상태라는건 메뉴가 켜져 있다는 것
		// 이때는 _Implementation 안붙여도 됨.
		RemovePauseMenu();
	}
	else {
		DisplayPauseMenu();
	}
}


void AMainPlayerController::GameModeOnly()
{
	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);
}

// ============================================ 인벤토리 ==============================
void AMainPlayerController::Interact()
{
	if (CurrentInteractable) {
		// CurrentInteractable의 존재여부는 Main.cpp에서 구현해 놓음
		CurrentInteractable->Interact(this);
	}
}

// 이미 Main에서 SetupInputComponent을 override해서 여기서 또 override하니까 오류뜸.
//void AMainPlayerController::SetupInputComponent()
//{
//	Super::SetupInputComponent();
//	
//	InputComponent->BindAction("Pickup", EInputEvent::IE_Pressed, this, &AMainPlayerController::Interact);
//}

void AMainPlayerController::AddItemToInventoryByID(FName ID)
{
	UE_LOG(LogTemp, Warning, TEXT("AddItemToInventoryByID() OK"));
	AFirstProject_cppGameModeBase* GameMode = Cast<AFirstProject_cppGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		UE_LOG(LogTemp, Warning, TEXT("GameMode OK"));
		UDataTable* ItemDB = GameMode->GetItemDB();
		
		FInventoryItem* ItemToAdd = ItemDB->FindRow<FInventoryItem>(ID, "");
		if (ItemToAdd) {
			Inventory.Add(*ItemToAdd);
		}
	}
}

void AMainPlayerController::Craftitem(FInventoryItem ItemA, FInventoryItem ItemB, AMainPlayerController* Controller)
{
	for (auto Craft : ItemB.CraftCombinations) {
		if (Craft.ComponentID == ItemA.ItemID) {
			if (Craft.bDestroyItemA) Inventory.RemoveSingle(ItemA);
			if (Craft.bDestroyItemB) Inventory.RemoveSingle(ItemB);
			AddItemToInventoryByID(Craft.ProductID);
			ReloadInventory(); // <-- 엔진 내부에서 작성
		}
	}
}