// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Critter.h"
#include "Enemy.h"
#include "AIController.h"
// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));

}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	if (Actor_1 && Actor_2 && Actor_3 && Actor_4) {
		SpawnArray.Add(Actor_1);
		SpawnArray.Add(Actor_2);
		SpawnArray.Add(Actor_3);
		SpawnArray.Add(Actor_4);
	}
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpawnVolume::GetSpawnPoint()
{
	// GetScaledBoxExtent() : Box의 Extent를 벡터로 반환
	FVector Extent = SpawningBox->GetScaledBoxExtent();
	// GetComponentLocation() : Box의 Origin 위치 벡터로 반환
	FVector Origin = SpawningBox->GetComponentLocation();
	// RandomPointInBoundingBox : 첫 번째 벡터를 원점으로 사용하고 두 번째 벡터를 상자 범위로 사용하여 지정된 경계 상자 내의 임의 점을 반환합니다.
	FVector Point = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
	return Point;
}

void ASpawnVolume::SpawnOurActor_Implementation(UClass* ToSpawn, const FVector& Location)
{
	if (ToSpawn) {
		UWorld* World = GetWorld();
		FActorSpawnParameters SpawnParams;

		if (World) {
			AActor* Actor = World->SpawnActor<AActor>(ToSpawn, Location, FRotator(0.f), SpawnParams);

			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if (Enemy) {
				// 이 Pawn에 대한 기본 컨트롤러를 생성하고 소유합니다.
				Enemy->SpawnDefaultController();

				AAIController* AICont = Cast<AAIController>(Enemy->GetController());
				if (AICont) {
					Enemy->AIController = AICont;
				}
			}
		}
	}
}

TSubclassOf<AActor> ASpawnVolume::GetSpawnActor()
{
	if (SpawnArray.Num() > 0) {
		int32 Selection = FMath::RandRange(0, SpawnArray.Num() - 1);
		return SpawnArray[Selection];
	}
	else {
		return nullptr;
	}
}