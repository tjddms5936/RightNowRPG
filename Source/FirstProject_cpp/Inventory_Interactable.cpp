// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory_Interactable.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Main.h"
#include "MainPlayerController.h"

// Sets default values
AInventory_Interactable::AInventory_Interactable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	//SphereComponent->SetupAttachment(GetRootComponent());
	//SphereComponent->InitSphereRadius(150.f);
}

// Called when the game starts or when spawned
void AInventory_Interactable::BeginPlay()
{
	Super::BeginPlay();
	//SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AInventory_Interactable::SphereComponentOverlapBegin);
	//SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AInventory_Interactable::SphereComponentOverlapEnd);
}

// Called every frame
void AInventory_Interactable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
//
//void AInventory_Interactable::SphereComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	// 이름만 띄우자. 그리고 E키 눌러서 먹을 수 있는건 Main.cpp에서 구현 ㄱ LineTrace 활용
//	AMain* Main = Cast<AMain>(OtherActor);
//	if (Main) {
//		bIsInRange = true;
//
//		Main->MainPlayerController->DisplayPickupText();
//		Main->MainPlayerController->bTextOnLevel = true;
//		Main->MainPlayerController->PickupItemLocation = GetActorLocation();
//		/*
//		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECollisionChannel::ECC_Visibility, QueryParams)) {
//			AInventory_Interactable* Interactable = Cast<AInventory_Interactable>(HitResult.GetActor()); < 부딫힐 필요 없이. 
//			if (Interactable) {
//				MainController->CurrentInteractable = Interactable;
//				MainController->bTextOnLevel = true;
//				MainController->PickupItemLocation = Interactable->GetActorLocation();
//				MainController->DisplayPickupText();
//				return;
//			}
//			else {
//				MainController->CurrentInteractable = nullptr;
//				MainController->bTextOnLevel = false;
//				MainController->RemovePickupText();
//				return;
//			}
//		}
//		else {
//			MainController->CurrentInteractable = nullptr;
//			MainController->bTextOnLevel = false;
//			MainController->RemovePickupText();
//			return;
//		}
//		*/
//	}
//}

//void AInventory_Interactable::SphereComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
// 
//}
