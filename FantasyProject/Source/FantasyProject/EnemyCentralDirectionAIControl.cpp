// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyCentralDirectionAIControl.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "EnemySoldier.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "PlayerControl.h"

namespace CentralDirectionTuning
{
	constexpr float SightDistance = 5000.0f;  // ved a 50 m
	constexpr float FireDistance = 4000.0f;  //spata a 40m
	constexpr float LoseSightExtraDistance = 200.0f;  // perdita di percezione.
	constexpr float RotationInterpSpeed = 6.0f;
}

AEnemyCentralDirectionAIControl::AEnemyCentralDirectionAIControl() :
	CentralFireDistance(CentralDirectionTuning::FireDistance),
	CentralSightDistance(CentralDirectionTuning::SightDistance),
	bCentralKeepAimUntilPlayerDead(false),
	bCentralBehaviorTreePaused(false)
	
	

{
}

void AEnemyCentralDirectionAIControl::BeginPlay()
{
	Super::BeginPlay();

	ConfigureCentralSight();
	BindCentralPerception();
}

void AEnemyCentralDirectionAIControl::OnPossess(APawn* InPawn)
{
	// Il padre inizializza il Blackboard e avvia il Behavior Tree di pattuglia.
	Super::OnPossess(InPawn);

	AEnemySoldier* Soldier = Cast<AEnemySoldier>(InPawn);
	if (!Soldier)
	{
		return;
	}

	bCentralKeepAimUntilPlayerDead = false;
	bCentralBehaviorTreePaused = false;
	HomeLocation = Soldier->GetActorLocation();

	if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
	{
		BlackboardComp->SetValueAsVector(TEXT("OfficeDirectionLocation"), HomeLocation);
	}

	// Super::OnPossess applica il raggio standard del controller padre (12 m).
	// Il Direction deve invece acquisire e sparare fino a 20 m.
	ConfigureCentralSight();
	BindCentralPerception();
}

void AEnemyCentralDirectionAIControl::ConfigureCentralSight()
{
	if (!SightConfig)
	{
		return;
	}

	SightConfig->SightRadius = CentralSightDistance;// CentralFireDistance;
	SightConfig->LoseSightRadius =CentralSightDistance + CentralDirectionTuning::LoseSightExtraDistance;//CentralFireDistance + CentralDirectionTuning::LoseSightExtraDistance;
	SightConfig->PeripheralVisionAngleDegrees = 150.0f;

	if (AIPerception)
	{
		AIPerception->ConfigureSense(*SightConfig);
		AIPerception->RequestStimuliListenerUpdate();
	}
}

void AEnemyCentralDirectionAIControl::BindCentralPerception()
{
	if (!AIPerception)
	{
		return;
	}

	// Super::BeginPlay collega la callback del controller standard, che contiene
	// Chase/MoveToActor. Il Direction deve usare esclusivamente questa callback.
	AIPerception->OnTargetPerceptionUpdated.RemoveAll(this);
	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this,&AEnemyCentralDirectionAIControl::OnCentralTargetPerceptionUpdated);
}

void AEnemyCentralDirectionAIControl::Tick(float DeltaSeconds)
{
	// Non chiamo Super::Tick: il Tick del controller padre esegue UpdateCombat
	// e puo emettere MoveToActor verso il Player. soltanto il Tick base AI.
	AAIController::Tick(DeltaSeconds);

	if (!bCentralKeepAimUntilPlayerDead)
	{
		return;
	}

	AEnemySoldier* Soldier = Cast<AEnemySoldier>(GetPawn());
	if (!Soldier)
	{
		return;
	}

	if (Soldier->IsDead())
	{
		bCentralKeepAimUntilPlayerDead = false;
		return;
	}

	APlayerControl* Player = Cast<APlayerControl>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!IsValid(Player) || Player->IsDead())
	{
		EndCentralSurveillance(Soldier);
		return;
	}

	
	//KeepCentralSoldierStopped(Soldier);
	//Soldier->SetAiming(true);
	//Soldier->SetCombatMovementMode(true);

	const FVector ToPlayer = Player->GetActorLocation() - Soldier->GetActorLocation();
	FRotator DesiredRotation = ToPlayer.Rotation();
	DesiredRotation.Pitch = 0.0f;
	DesiredRotation.Roll = 0.0f;

	//FRotator SmoothRotation = FMath::RInterpTo(GetControlRotation(),DesiredRotation,DeltaSeconds,CentralDirectionTuning::RotationInterpSpeed);
	FRotator SmoothRotation = FMath::RInterpTo(Soldier->GetActorRotation(), DesiredRotation, DeltaSeconds, 3.0f);
	SmoothRotation.Pitch = 0.0f;
	SmoothRotation.Roll = 0.0f;
	Soldier->SetActorRotation(SmoothRotation);

	const float DistanceToPlayer = FVector::Dist2D(Soldier->GetActorLocation(),Player->GetActorLocation());

	// FireAtTarget esegue anche il trace dalla volata e rispetta FireInterval.
	// LineOfSightTo evita ogni tentativo di fuoco quando il Player e coperto.
	if (DistanceToPlayer <= CentralFireDistance && LineOfSightTo(Player))
	{
		Soldier->FireAtTarget(Player);
	}
}

void AEnemyCentralDirectionAIControl::OnCentralTargetPerceptionUpdated(
	AActor* Actor,
	FAIStimulus Stimulus)
{
	APlayerControl* Player = Cast<APlayerControl>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!IsValid(Player) || Actor != Player || Player->IsDead())
	{
		return;
	}

	// La perdita successiva della vista non annulla la sorveglianza: il requisito
	// prevede che la mira termini soltanto quando il Player muore.
	if (!Stimulus.WasSuccessfullySensed())
	{
		return;
	}

	if (AEnemySoldier* Soldier = Cast<AEnemySoldier>(GetPawn()))
	{
		StartCentralSurveillance(Soldier, Player);
	}
}

void AEnemyCentralDirectionAIControl::StartCentralSurveillance(AEnemySoldier* Soldier,APlayerControl* Player)
{
	if (!Soldier || !Player)
	{
		return;
	}
	if (bCentralKeepAimUntilPlayerDead)
	{
		return;
	}
	bCentralKeepAimUntilPlayerDead = true;

	// annulla il path corrente, ma il Behavior Tree puo creare subito un altro MoveTo.
	if (!bCentralBehaviorTreePaused)
	{
		if (UBrainComponent* Brain = GetBrainComponent())
		{
			Brain->StopLogic(TEXT("Direction surveillance: stationary until Player death"));
			bCentralBehaviorTreePaused = true;
		}
	}

	KeepCentralSoldierStopped(Soldier); // stop una sola volta
	//Soldier->SetCombatMovementMode(true);
	Soldier->SetAiming(true);

	if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
	{
		BlackboardComp->SetValueAsObject(TargetActorKeyName, Player);
		BlackboardComp->SetValueAsBool(InAttackRangeKeyName, true);
	}
}

void AEnemyCentralDirectionAIControl::EndCentralSurveillance(AEnemySoldier* Soldier)
{
	bCentralKeepAimUntilPlayerDead = false;

	StopMovement();
	ClearFocus(EAIFocusPriority::Gameplay);

	if (Soldier)
	{
		if (UCharacterMovementComponent* Movement = Soldier->GetCharacterMovement())
		{
			Movement->StopMovementImmediately();
			Movement->MaxWalkSpeed = Soldier->GetWalkSpeed();
		}

		Soldier->SetAiming(false);
		Soldier->SetCombatMovementMode(false);
	}

	// Pulire il Blackboard prima del riavvio impedisce al BT di usare per un frame
	// il vecchio bersaglio morto.
	if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
	{
		BlackboardComp->ClearValue(TargetActorKeyName);
		BlackboardComp->SetValueAsBool(InAttackRangeKeyName, false);
	}

	if (bCentralBehaviorTreePaused)
	{
		if (UBrainComponent* Brain = GetBrainComponent())
		{
			Brain->RestartLogic();
		}

		bCentralBehaviorTreePaused = false;
	}
}

void AEnemyCentralDirectionAIControl::KeepCentralSoldierStopped(AEnemySoldier* Soldier)
{
	StopMovement();

	if (Soldier)
	{
		if (UCharacterMovementComponent* Movement = Soldier->GetCharacterMovement())
		{
			Movement->StopMovementImmediately();
		}
	}
}
