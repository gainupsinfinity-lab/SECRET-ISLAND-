// Fill out your copyright notice in the Description page of Project Settings.


#include "TowerGateController.h"
#include "EnemySoldier.h"
#include "TimerManager.h"


ATowerGateController::ATowerGateController()
{
 
	PrimaryActorTick.bCanEverTick = false;

	bGateOpened = false;

	RequiredEnemies = 3;

}


void ATowerGateController::BeginPlay()
{
	Super::BeginPlay();
	// Controlla i 3 nemici ogni 0.25 secondi.
	GetWorldTimerManager().SetTimer(CheckEnemiesTimerHandle,this,&ATowerGateController::CheckTowerEnemies,0.25f,true);
	
}
void ATowerGateController::CheckTowerEnemies()
{
	// Evita di aprire due volte.
	if (bGateOpened)
	{
		return;
	}

	// Devono essere assegnati esattamente 3 nemici.
	//if (TowerEnemies.Num() != 3)
	if (TowerEnemies.Num() != RequiredEnemies)
	{
		return;
	}

	// Controlla tutti e tre.
	for (AEnemySoldier* Enemy : TowerEnemies)
	{
		if (IsValid(Enemy) && !Enemy->IsDead())
		{
			return;
		}

	}
	

	// Enemy 1 morto
	// Enemy 2 morto
	// Enemy 3 morto open

	bGateOpened = true;

	GetWorldTimerManager().ClearTimer(CheckEnemiesTimerHandle);

	OnAllTowerEnemiesDead();
}
