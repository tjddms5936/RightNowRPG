// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FirstProject_cppGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_CPP_API AFirstProject_cppGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	

public:

	class UDataTable* GetItemDB() const { return ItemDB; }

protected:
	UPROPERTY(EditDefaultsOnly)
	UDataTable* ItemDB;
};
