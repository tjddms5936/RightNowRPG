// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "Inventory_Interactable.h"
#include "FirstProject_cppGameModeBase.h"
#include "Blueprint/UserWidget.h"


void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();
	/* ���� ������ �־� �ݵ�� ������ ���� ���� ��Ʈ�ѷ� ����
	1. ���� ���� �����Ͽ���
	2. ���� �������ֱ�
	*/

	if (HUDOverlayAsset) {
		UE_LOG(LogTemp, Warning, TEXT("HUDOverlayAsset OK"));
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);

		// �� ������ ����Ʈ�� �߰� �ϱ� ���� �۾�
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
		FVector2D PositionInViewport; // ���� ��ġ ����
		// World Space 3D ��ġ�� 2D Screen Space ��ġ�� ��ȯ�մϴ�.
		// ���� ��ǥ�� ȭ�鿡 ���������� ������ ��� true ������
		ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport);
		PositionInViewport.Y -= 85.f;
		PositionInViewport.X -= 120.f;


		FVector2D SizeInViewport(350.f, 25.f); // ���� ������ ����

		EnemyHealthBar->SetPositionInViewport(PositionInViewport);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
	}

	if (BossEnemyHealthBar) {
		FVector2D PositionInViewport;
		ProjectWorldLocationToScreen(BossEnemyLocation, PositionInViewport); // 3D��ġ���� 2D�� �ٲ��ִ� �۾�. 
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
		// ���� ������ �������Ʈ���� ������ �ְ� �������ν� �ִϸ��̼� �����۵�����

	}
}

void AMainPlayerController::TogglePauseMenu()
{
	if (bPauseMenuVisible) {
		// true���¶�°� �޴��� ���� �ִٴ� ��
		// �̶��� _Implementation �Ⱥٿ��� ��.
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

// ============================================ �κ��丮 ==============================
void AMainPlayerController::Interact()
{
	if (CurrentInteractable) {
		// CurrentInteractable�� ���翩�δ� Main.cpp���� ������ ����
		CurrentInteractable->Interact(this);
	}
}

// �̹� Main���� SetupInputComponent�� override�ؼ� ���⼭ �� override�ϴϱ� ������.
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
			ReloadInventory(); // <-- ���� ���ο��� �ۼ�
		}
	}
}