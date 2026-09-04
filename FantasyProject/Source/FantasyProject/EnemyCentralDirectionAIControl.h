// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemySoldierAIController.h"
#include "EnemyCentralDirectionAIControl.generated.h"

class AEnemySoldier;
class APlayerControl;

/**
 * Controller del soldato Direction:
 * - pattuglia tramite il Behavior Tree prima di vedere il Player;
 * - al primo avvistamento interrompe la pattuglia e resta fermo;
 * - continua a ruotare/mirare il Player fino alla sua morte;
 * - spara soltanto quando il Player e visibile ed entro CentralFireDistance.
 */
UCLASS()
class FANTASYPROJECT_API AEnemyCentralDirectionAIControl : public AEnemySoldierAIController
{
	GENERATED_BODY()

public:
	AEnemyCentralDirectionAIControl();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void OnCentralTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	/** Distanza massima di fuoco: 2000 cm = 20 metri. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction AI|Combat", meta = (ClampMin = "100.0"))
	float CentralFireDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction AI| Percepition", meta = (ClampMin = "100.0"))
		float CentralSightDistance;

private:
	void ConfigureCentralSight();
	void BindCentralPerception();
	void StartCentralSurveillance(AEnemySoldier* Soldier, APlayerControl* Player);
	void EndCentralSurveillance(AEnemySoldier* Soldier);
	void KeepCentralSoldierStopped(AEnemySoldier* Soldier);

	bool bCentralKeepAimUntilPlayerDead;
	bool bCentralBehaviorTreePaused;
};
