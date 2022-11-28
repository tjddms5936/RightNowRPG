// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTransitionVolume.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"
#include "Main.h"
#include "Kismet/KismetSystemLibrary.h"
// Sets default values
ALevelTransitionVolume::ALevelTransitionVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; // �츰 tick���� �ʿ� x �״ϱ� false

	TransitionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TransitionVolume"));
	RootComponent = TransitionVolume;

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(GetRootComponent());

	TransitionLevelName = "SunTemple";
}

// Called when the game starts or when spawned
void ALevelTransitionVolume::BeginPlay()
{
	Super::BeginPlay();
	
	// overlap����� ����ϱ� ����
	TransitionVolume->OnComponentBeginOverlap.AddDynamic(this, &ALevelTransitionVolume::OnOverlapBegin);
}

// Called every frame
void ALevelTransitionVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelTransitionVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) {

		AMain* Main = Cast<AMain>(OtherActor);
		if (Main) {
			if (TransitionLevelName == FName("Level2_HuntingGround")) {
				// Level1�� Ŭ���� �Ǿ�� ��
				if (!Main->bIsLevel1Clear) {
					UKismetSystemLibrary::PrintString(this, FString("You can't enter the Level2."));
					UKismetSystemLibrary::PrintString(this, FString("You need to clear the Level1."));
					return;
				}
			}
			if (TransitionLevelName == FName("Level3_HuntingGround")) {
				// Level2�� Ŭ���� �Ǿ�� ��
				if (!Main->bIsLevel2Clear) {
					UKismetSystemLibrary::PrintString(this, FString("You can't enter the Level3."));
					UKismetSystemLibrary::PrintString(this, FString("You need to clear the Level2."));
					return;
				}
			}
			Main->SaveGame(); // ��ġ�� SaveGame���� ���ְ�
			Main->SwitchLevel(TransitionLevelName); // ���� �̵��ϸ鼭 SwitchLevel�Լ����� LoadGame�� �ߵ���
		}
	}
}
