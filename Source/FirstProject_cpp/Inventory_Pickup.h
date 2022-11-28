// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory_Interactable.h"
#include "Inventory_Pickup.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_CPP_API AInventory_Pickup : public AInventory_Interactable
{
	GENERATED_BODY()
	
public:
	AInventory_Pickup();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Pickup")
	void OnUsed(class AMain* MainCharacterReferece);

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* PickupMesh;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PickupItem)
	FName ItemID;
};
