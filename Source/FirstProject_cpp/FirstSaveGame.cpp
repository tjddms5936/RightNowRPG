// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstSaveGame.h"

UFirstSaveGame::UFirstSaveGame() {

	PlayerName = TEXT("Default");
	UserIndex = 0;

	// 디폴트로 빈문자 
	CharacterStats.WeaponName = TEXT("");
	CharacterStats.LevelName = TEXT("");
	CharacterStats.SkillNameArray.SetNum(9);
	CharacterStats.SkillThumbnailArray.SetNum(9);
}