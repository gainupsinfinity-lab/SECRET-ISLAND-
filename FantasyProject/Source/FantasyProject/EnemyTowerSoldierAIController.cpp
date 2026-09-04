// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyTowerSoldierAIController.h"
#include "EnemySoldier.h"
#include "PlayerControl.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"




AEnemyTowerSoldierAIController::AEnemyTowerSoldierAIController()
{
    TowerMaxHomeDistance = 800.0f;
    bTowerKeepAimUntilPlayerDead = false;

}

void AEnemyTowerSoldierAIController::BeginPlay() 
{
    Super::BeginPlay();
    if (SightConfig)
    {
        SightConfig->PeripheralVisionAngleDegrees = 100.0f;  // cisuale + ampia
        if (AIPerception)
        {
            AIPerception->ConfigureSense(*SightConfig);
            AIPerception->RequestStimuliListenerUpdate();
        }
    }
    if (AIPerception)  // se il player lo vede almeno una volta
    {
       // AIPerception->OnTargetPerceptionUpdated.RemoveDynamic(this,&AEnemyTowerSoldierAIController::OnTowerTargetPerceptionUpdated);

        //AIPerception->OnTargetPerceptionUpdated.AddDynamic(this,&AEnemyTowerSoldierAIController::OnTowerTargetPerceptionUpdated);

        AIPerception->OnTargetPerceptionUpdated.RemoveAll(this);

        AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyTowerSoldierAIController::OnTowerTargetPerceptionUpdated);
    }
}

void AEnemyTowerSoldierAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    AEnemySoldier* Soldier = Cast<AEnemySoldier>(InPawn);

    if (!Soldier)
    {
        return;
    }

    HomeLocation = Soldier->GetActorLocation();

    if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
    {
        BlackboardComp->SetValueAsVector(TEXT("HomeTowerLocation"), HomeLocation
        );
    }
}

void AEnemyTowerSoldierAIController::OnTowerTargetPerceptionUpdated(AActor* Actor,FAIStimulus Stimulus)  // evento percezione
{
    if (!IsValid(Actor))
    {
        return;
    }
    AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(this, 0);

    if (Actor != PlayerActor)  // solo il player
    {
        return;
    }
    if (Stimulus.WasSuccessfullySensed())
    {
        // Il Tower ha visto il Player almeno una volta.
        bTowerKeepAimUntilPlayerDead = true;  // tiene la mira
    }

    AEnemySoldierAIController ::OnTargetPerceptionUpdated(Actor, Stimulus);
   

   

    
}

void AEnemyTowerSoldierAIController::Tick(float DeltaSeconds)
{
    // Movimento, Chase, MovingFire, Attack ecc.
    // restano quelli già funzionanti del controller base.
    Super::Tick(DeltaSeconds);


    if (!bTowerKeepAimUntilPlayerDead)
    {
        return;
    }


    AEnemySoldier* Soldier = Cast<AEnemySoldier>(GetPawn());

    APlayerControl* Player =Cast<APlayerControl>(UGameplayStatics::GetPlayerCharacter(this, 0));


    if (!Soldier  || !IsValid(Player))
    {
        return;
    }


    // Se muore il Tower non c'è più niente da mantenere.
    if (Soldier->IsDead())
    {
        bTowerKeepAimUntilPlayerDead = false;
        return;
    }



    // Il Player è morto:
    // termina definitivamente la mira persistente.
    if (Player->IsDead())
    {
        bTowerKeepAimUntilPlayerDead = false;
        Soldier->SetAiming(false);
    }


    
    
    // Soldier->SetAiming(true);
    //
    // perché il controller base potrebbe nello stesso frame
    // avere impostato SetAiming(false), causando
    // TRUE -> FALSE -> TRUE e il piccolo freeze.
}


bool AEnemyTowerSoldierAIController::MustAbandonTargetForSpecialeRules(AEnemySoldier* Soldier,AActor* TargetActor) const
{
    if (!Soldier || !IsValid(TargetActor))
    {
        return false;
    }


    // Distanza DEL TOWER dalla propria posizione iniziale.
    const float EnemyDistanceFromHome =FVector::Dist2D(HomeLocation,Soldier->GetActorLocation());


    // Il Tower non può allontanarsi oltre
    // il proprio territorio.
    if (EnemyDistanceFromHome > TowerMaxHomeDistance)
    {
        return true;
    }


    return false;
}


bool AEnemyTowerSoldierAIController::ShouldKeepAimingForSpecialeRules() const  // per dire setAiming(false)
{
    return bTowerKeepAimUntilPlayerDead;
}

