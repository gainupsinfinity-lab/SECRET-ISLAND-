// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemySoldierAIController.h"
#include "EnemyTowerSoldierAIController.generated.h"

/**
 * 
 */
UCLASS()
class FANTASYPROJECT_API AEnemyTowerSoldierAIController : public AEnemySoldierAIController
{
	GENERATED_BODY()


public:


    AEnemyTowerSoldierAIController();



protected:

    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION()
        void OnTowerTargetPerceptionUpdated(AActor* Actor,FAIStimulus Stimulus);

    bool bTowerKeepAimUntilPlayerDead;

    virtual bool MustAbandonTargetForSpecialeRules(AEnemySoldier* Soldier,AActor* TargetActor) const override;

    virtual bool ShouldKeepAimingForSpecialeRules() const override;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Tower AI|Territory",meta = (ClampMin = "100.0"))
        float TowerMaxHomeDistance;
	
};
