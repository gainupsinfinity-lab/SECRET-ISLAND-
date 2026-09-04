// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TowerGateController.generated.h"

class AEnemySoldier;


UCLASS()
class FANTASYPROJECT_API ATowerGateController : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ATowerGateController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// I 3 soldati che proteggono la torre.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gate")
		TArray<AEnemySoldier*> TowerEnemies;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gate", meta = (ClampMin = "1"))
		int32 RequiredEnemies;

	// Evento che partirà UNA SOLA VOLTA quando i 3 nemici sono morti.
	UFUNCTION(BlueprintImplementableEvent, Category = "Gate")
		void OnAllTowerEnemiesDead();


private:
	void CheckTowerEnemies();

	FTimerHandle CheckEnemiesTimerHandle;

	bool bGateOpened;

};
