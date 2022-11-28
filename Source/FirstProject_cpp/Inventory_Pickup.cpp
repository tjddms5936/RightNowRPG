// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory_Pickup.h"
#include "Components/StaticMeshComponent.h"

AInventory_Pickup::AInventory_Pickup()
{
	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetupAttachment(GetRootComponent());

	ItemID = FName("Please enter an ID");

}
