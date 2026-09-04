// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySoldierAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "EnemySoldier.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Engine/Engine.h"
#include "PlayerControl.h"

namespace EnemyCombatTuning
{
	constexpr float StopAndFireDistance = 600.0f;  //6m 
	constexpr float StartMovingFireDistance = 2200.0f;  //22m
	constexpr float SightDistance = 2500.0f;  // vede a 25m
	constexpr float LoseSightDistance = 2700.0f; //perde a 27m
	constexpr float MaxEngagementDistance = 2700.0f; //perde a 27
	constexpr float PeripheralVisionAngle = 85.0f;
	constexpr float MovingFireWalkSpeed = 280.0f;
}

AEnemySoldierAIController::AEnemySoldierAIController() :
	BehaviorTreeAsset(nullptr),
	AIPerception(nullptr),
	SightConfig(nullptr),
	AttackEnterDistance(EnemyCombatTuning::StopAndFireDistance),
	AttackExitDistance(EnemyCombatTuning::StopAndFireDistance),
	MovingFireStartDistance(EnemyCombatTuning::StartMovingFireDistance),
	MaxEngagmentDistance(EnemyCombatTuning::MaxEngagementDistance),
	SightDistance(EnemyCombatTuning::SightDistance),
	LoseSightDistance(EnemyCombatTuning::LoseSightDistance),
	PeripheralVisionAngleDegrees(EnemyCombatTuning::PeripheralVisionAngle),
	MoveAcceptanceRadius(75.0f),
	CombatMoveAcceptRadius(500.0f),
	LostSightGraceTime(6.0f), //aspetta 6 sec pirma di abbandonare
	AimToleranceDegrees(8.0f),
	AimRotationInterpSpeed(4.0f),
	FirstShotDelay(0.25f),
	TargetActorKeyName(TEXT("TargetActor")),
	InAttackRangeKeyName(TEXT("InAttackRange")),
	CombatState(EEnemyCombatState::Patrol),
	MaxHomeDistance(1000.0f),
	bUseHomeLimit(false),
	HomeLocation(FVector::ZeroVector),
	LastKnownTargetLocation(FVector::ZeroVector),
	LostSightElapsed(0.0f),
	AttackStateElapsed(0.0f),
	bHasLineOfSight(false),
	bInAttackRange(false),
	bBehaviorTreePaused(false),
	bKeepAimWhilePlayerInRange(false)
	
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.05f;

	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->SightRadius = SightDistance;
	SightConfig->LoseSightRadius = LoseSightDistance;
	SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngleDegrees;
	SightConfig->SetMaxAge(2.0f);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
	SetPerceptionComponent(*AIPerception);
}

void AEnemySoldierAIController::BeginPlay()
{
	Super::BeginPlay();

	if (AIPerception)  // ogni volta che la percezione AI vede osmette di vedere qualcosa chima la funzione 
	{
		AIPerception->OnTargetPerceptionUpdated.RemoveDynamic(this,&AEnemySoldierAIController::OnTargetPerceptionUpdated);
		AIPerception->OnTargetPerceptionUpdated.AddDynamic(this,&AEnemySoldierAIController::OnTargetPerceptionUpdated);
	}
}

void AEnemySoldierAIController::OnPossess(APawn* InPawn)  // prende il controllo del Pawn
{
	Super::OnPossess(InPawn);

	AEnemySoldier* Soldier = Cast<AEnemySoldier>(InPawn);
	if (!Soldier)
	{
		UE_LOG(LogTemp, Error, TEXT("EnemyAIController: il Pawn non e un AEnemySoldier."));
		return;
	}

	//applica valori correnti del BP
	ApplyRequiredCombatTuning();

	HomeLocation = Soldier->GetActorLocation(); // memorizzazione pos iniziale

	Soldier->SetAiming(false);
	Soldier->SetCombatMovementMode(false);

	if (BehaviorTreeAsset)
	{
		const bool bStarted = RunBehaviorTree(BehaviorTreeAsset);
		if (bStarted)
		{
			UE_LOG(LogTemp,Log,TEXT("EnemyAIController: Behavior Tree %s avviato."),*GetNameSafe(BehaviorTreeAsset));
		}
		else
		{
			UE_LOG(LogTemp,Error,TEXT("EnemyAIController: avvio del Behavior Tree %s fallito."),*GetNameSafe(BehaviorTreeAsset));
		}
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("EnemyAIController: BehaviorTreeAsset non assegnato; il combattimento C++ funziona comunque."));
	}

	WriteBlackboardState();
}

void AEnemySoldierAIController::OnUnPossess()
{
	StopMovement();
	ClearFocus(EAIFocusPriority::Gameplay);
	CurrentTarget.Reset();
	bHasLineOfSight = false;
	bInAttackRange = false;
	bBehaviorTreePaused = false;
	bKeepAimWhilePlayerInRange = false; // cancella lo stato di mira quando controller non controlla + nemico

	Super::OnUnPossess();
}

void AEnemySoldierAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateCombat(DeltaSeconds);
	
	if (!bKeepAimWhilePlayerInRange)
	{
		return;
	}
	AEnemySoldier* Soldier =Cast<AEnemySoldier>(GetPawn());
	
	APlayerControl* Player =Cast<APlayerControl>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if (!Soldier || !IsValid(Player))
	{
		return;
	}
	const float DistanceToPlayer =FVector::Dist2D(Soldier->GetActorLocation(),Player->GetActorLocation());
	
	if (!Player->IsDead() && DistanceToPlayer <= MaxEngagmentDistance)
	{
		Soldier->SetAiming(true);
	}
	else
	{
		bKeepAimWhilePlayerInRange = false;
		Soldier->SetAiming(ShouldKeepAimingForSpecialeRules());
	}
		
}
	

void AEnemySoldierAIController::OnTargetPerceptionUpdated(AActor* Actor,FAIStimulus Stimulus)
{
	if (!IsValid(Actor))
	{
		return;
	}

	AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(this, 0);
	if (Actor != PlayerActor)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		const bool bNewTarget = CurrentTarget.Get() != Actor;
		CurrentTarget = Actor;
		//bKeepAimUnitlPlayerDead = true; //****** vede il player almeno 1 volta//  
		bKeepAimWhilePlayerInRange = true;
		LastKnownTargetLocation = Actor->GetActorLocation();
		LostSightElapsed = 0.0f;
		bHasLineOfSight = true;

		PauseBehaviorTreeForCombat();

		if (bNewTarget || CombatState == EEnemyCombatState::Patrol ||CombatState == EEnemyCombatState::Investigate)
		{
			bInAttackRange = false;
			SetCombatState(EEnemyCombatState::Chase);
		}

		UE_LOG(LogTemp, Log, TEXT("ENEMY: player visto (%s)."), *GetNameSafe(Actor));
	}
	else if (CurrentTarget.Get() == Actor)
	{
		bHasLineOfSight = false;
		LostSightElapsed = 0.0f;
		LastKnownTargetLocation = Actor->GetActorLocation();
	

		UE_LOG(LogTemp, Log, TEXT("ENEMY: visuale persa, controllo ultima posizione."));
	}

	WriteBlackboardState();
}

void AEnemySoldierAIController::UpdateCombat(float DeltaSeconds)
{
	AEnemySoldier* Soldier = Cast<AEnemySoldier>(GetPawn());

	if (!Soldier || Soldier->IsDead())
	{
		return;
	}
	

	// Controlla che il bersaglio sia ancora valido.
	if (!IsCurrentTargetValid())
	{
		if (CombatState != EEnemyCombatState::Patrol ||CurrentTarget.IsValid())
		{
			ForgetTarget();
		}

		return;
	}


	AActor* TargetActor = CurrentTarget.Get();//bersaglio che insegue

	const float Distance =FVector::Dist2D(Soldier->GetActorLocation(),TargetActor->GetActorLocation());  //distanza nemivo player


	// =========================================================
	// OLTRE 27 METRI:
	// abbandona il Player .
	// =========================================================

	if (Distance > MaxEngagmentDistance)
	{
		ForgetTarget();
		return;
	}
	if (MustAbandonTargetForSpecialeRules(Soldier, TargetActor))
	{
		ForgetTarget();
		return;
	}


	// =========================================================
	// PERDITA TEMPORANEA DELLA VISUALE
	// =========================================================

	if (!bHasLineOfSight)
	{
		LostSightElapsed += DeltaSeconds;


		// Se la perdita della visuale dura meno del Grace Time,
		// NON cambia stato.
		//
		// Quindi:
		// - non passa a Investigate
		// - non mette Aiming a false
		// - non apre le braccia per una micro-perdita di visuale.
		if (LostSightElapsed < LostSightGraceTime)
		{
			WriteBlackboardState();
			return;
		}


		// Se invece la visuale manca realmente troppo a lungo,
		// abbandona il bersaglio.
		bInAttackRange = false;

		ForgetTarget();

		return;
	}


	// =========================================================
	// PLAYER VISIBILE
	// =========================================================

	LostSightElapsed = 0.0f;

	LastKnownTargetLocation =TargetActor->GetActorLocation();


	// =========================================================
	// CONTROLLO DISTANZA ATTACK = 6m
	// =========================================================

	if (bInAttackRange)
	{
		// Se era dentro Attack ma il Player supera i 6 metri,
		// esce dalla zona di Attack.
		if (Distance > AttackExitDistance)
		{
			bInAttackRange = false;
		}
	}
	else if (Distance <= AttackEnterDistance)
	{
		// Se arriva entro 6 metri,
		// entra nella zona Attack.
		bInAttackRange = true;
	}


	// =========================================================
	// ATTACK
	//
	// ENTRO 6 METRI:
	// fermo + mira + spara dopo First Delay
	// =========================================================

	if (bInAttackRange)
	{
		SetCombatState(EEnemyCombatState::Attack);

		UpdateAttack(DeltaSeconds);
	}


	// =========================================================
	// Da 6 METRI A 22 Avanaza e Spara speed +lento 
	// =========================================================

	else if(Distance <= MovingFireStartDistance)
	{
		SetCombatState(EEnemyCombatState::MovingFire);

		if (UCharacterMovementComponent* Movement = Soldier->GetCharacterMovement())
		{
			Movement->MaxWalkSpeed = EnemyCombatTuning::MovingFireWalkSpeed;//Soldier->GetWalkSpeed();//MovingFireWalkSpeed;
		}
		Soldier->SetAiming(true);
		Soldier->SetCombatMovementMode(true);
		FaceTarget(TargetActor, DeltaSeconds);
		// Continua ad avanzare verso il Player.
		if (GetMoveStatus() != EPathFollowingStatus::Moving)
		{															//false
			MoveToActor(TargetActor,CombatMoveAcceptRadius,false,true,true,nullptr,true);
			
		}
			// Spara mentre avanza.
		Soldier->FireAtTarget(TargetActor);
		
	}
   ///// DA 22m A 27 m-> inseguokmeto senza sparare
	else
	{
		if (UCharacterMovementComponent* Movement =Soldier->GetCharacterMovement())
		{
		Movement->MaxWalkSpeed =Soldier->GetWalkSpeed();
		}
		SetCombatState(EEnemyCombatState::Chase);
		UpdateChase(Soldier, TargetActor);
	}
	
	// Aggiorna il Blackboard alla fine della gestione Combat.
	WriteBlackboardState();
}



void AEnemySoldierAIController::SetCombatState(EEnemyCombatState NewState)
{
	if (CombatState == NewState)
	{
		return;
	}

	CombatState = NewState;
	if (CombatState == EEnemyCombatState::Attack)
	{
		AttackStateElapsed = 0.0f;  // parte timer da zero ogni volta che entra nello stato attack
	}
	AEnemySoldier* Soldier = Cast<AEnemySoldier>(GetPawn());
	if (!Soldier)
	{
		return;
	}

	switch (CombatState)
	{
	case EEnemyCombatState::Patrol:  // BT torna a gestire il pattugliamento
		StopMovement();
		ClearFocus(EAIFocusPriority::Gameplay);
		Soldier->GetCharacterMovement()->MaxWalkSpeed = Soldier->GetWalkSpeed();
		Soldier->SetAiming(ShouldKeepAimingForSpecialeRules());
		Soldier->SetCombatMovementMode(false);
		ResumeBehaviorTreeAfterCombat();
		break;

	case EEnemyCombatState::Chase:
		PauseBehaviorTreeForCombat();
		StopMovement();
		ClearFocus(EAIFocusPriority::Gameplay);
		Soldier->GetCharacterMovement()->MaxWalkSpeed = Soldier->GetWalkSpeed();
		Soldier->SetAiming(ShouldKeepAimingForSpecialeRules());
		//Soldier->SetAiming(true);
		Soldier->SetCombatMovementMode(false);
		if (AActor* TargetActor = CurrentTarget.Get())
		{
			MoveToActor(TargetActor, CombatMoveAcceptRadius,false,true,false,nullptr,true);
		}
		break;

	case EEnemyCombatState::MovingFire:
		PauseBehaviorTreeForCombat();
		ClearFocus(EAIFocusPriority::Gameplay);
		Soldier->GetCharacterMovement()->MaxWalkSpeed = EnemyCombatTuning::MovingFireWalkSpeed;// Soldier->GetWalkSpeed();// MovingFireWalkSpeed;
		Soldier->SetAiming(true);
		Soldier->SetCombatMovementMode(true);
		
		break;

	case EEnemyCombatState::Attack:
		PauseBehaviorTreeForCombat();
		StopMovement();
		Soldier->GetCharacterMovement()->StopMovementImmediately();
	//	SetControlRotation(Soldier->GetActorRotation());
		Soldier->SetCombatMovementMode(true);
		Soldier->SetAiming(true);
		break;

	case EEnemyCombatState::Investigate:
		PauseBehaviorTreeForCombat();
		StopMovement();
		ClearFocus(EAIFocusPriority::Gameplay);
		Soldier->GetCharacterMovement()->MaxWalkSpeed = Soldier->GetWalkSpeed();
		Soldier->SetAiming(ShouldKeepAimingForSpecialeRules());
		Soldier->SetCombatMovementMode(false);
		MoveToLocation(LastKnownTargetLocation,MoveAcceptanceRadius,false,true,false,true,nullptr,true);
		break;
	}
}

void AEnemySoldierAIController::UpdateChase(AEnemySoldier* Soldier,AActor* TargetActor)
{
	Soldier->SetAiming(ShouldKeepAimingForSpecialeRules());
	//Soldier->SetAiming(true);
	Soldier->SetCombatMovementMode(false);

	if (GetMoveStatus() != EPathFollowingStatus::Moving)
	{
		MoveToActor(TargetActor, CombatMoveAcceptRadius,false,true,false,nullptr,true);
	}
}

void AEnemySoldierAIController::UpdateAttack(float DeltaSeconds)  //gestice l'attacco ravvicinato
{
	AEnemySoldier* EnemyPawn =Cast<AEnemySoldier>(GetPawn());

	AActor* TargetActor = CurrentTarget.Get();

	if (!EnemyPawn || !IsValid(TargetActor))
	{
		ForgetTarget();
		return;
	}

	StopMovement();

	
	// orientiare il nemico gradualmente.
	ClearFocus(EAIFocusPriority::Gameplay);

	FaceTarget(TargetActor, DeltaSeconds);

	AttackStateElapsed += DeltaSeconds;

	// Il nemico aspetta prima di effettuare il primo colpo.
	if (AttackStateElapsed < FirstShotDelay)
	{
		return;
	}

	const FVector ToTarget =TargetActor->GetActorLocation() -EnemyPawn->GetActorLocation();

	const FRotator DesiredRotation =ToTarget.Rotation();

	const float YawError =FMath::Abs(FMath::FindDeltaAngleDegrees(EnemyPawn->GetActorRotation().Yaw,DesiredRotation.Yaw));

	if (YawError <= AimToleranceDegrees)
	{
		EnemyPawn->FireAtTarget(TargetActor);
	}
}

void AEnemySoldierAIController::UpdateInvestigate(AEnemySoldier* Soldier)
{
	Soldier->SetAiming(ShouldKeepAimingForSpecialeRules());
	Soldier->SetCombatMovementMode(false);

	const float DistanceToLastKnown = FVector::Dist2D(Soldier->GetActorLocation(),LastKnownTargetLocation);

	if (DistanceToLastKnown > MoveAcceptanceRadius &&
		GetMoveStatus() != EPathFollowingStatus::Moving)
	{
		MoveToLocation(LastKnownTargetLocation,MoveAcceptanceRadius,true,true,false,true,nullptr,true);
	}
}

void AEnemySoldierAIController::FaceTarget(AActor* TargetActor, float DeltaSeconds)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn || !TargetActor)
	{
		return;
	}

	FVector AimPoint = TargetActor->GetActorLocation();
	if (const ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor))
	{
		AimPoint.Z += TargetCharacter->BaseEyeHeight * 0.5f;
	}

	FRotator DesiredRotation =(AimPoint - ControlledPawn->GetActorLocation()).Rotation();
	DesiredRotation.Roll = 0.0f;

	FRotator SmoothedRotation = FMath::RInterpTo(GetControlRotation(),DesiredRotation,DeltaSeconds,AimRotationInterpSpeed);
	SmoothedRotation.Roll = 0.0f;

	SetControlRotation(SmoothedRotation);
}

void AEnemySoldierAIController::ApplyRequiredCombatTuning()
{
	AttackExitDistance = AttackEnterDistance;
	
	

	if (SightConfig)
	{
		SightConfig->SightRadius = SightDistance;
		SightConfig->LoseSightRadius = LoseSightDistance;
		SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngleDegrees;

		if (AIPerception)
		{
			AIPerception->ConfigureSense(*SightConfig);
			AIPerception->RequestStimuliListenerUpdate();
		}
	}

	UE_LOG(LogTemp, Log,
		TEXT("ENEMY schema: vede %.0f cm | perde /abbandona  %.0f/ %.0f cm |spara avanzando %.0f cm | stop+spara %.0f cm | velocita %.2f s"),
		SightDistance,LoseSightDistance,MaxEngagmentDistance, MovingFireStartDistance, AttackEnterDistance,LostSightGraceTime);
}

void AEnemySoldierAIController::PauseBehaviorTreeForCombat()
{
	if (bBehaviorTreePaused)
	{
		return;
	}
	
	if (UBrainComponent* Brain = GetBrainComponent())
	{
		Brain->StopLogic(TEXT("Combat handled by EnemySoldierAIController"));
		bBehaviorTreePaused = true;
		
	}
	
}

void AEnemySoldierAIController::ResumeBehaviorTreeAfterCombat()
{
	if (!bBehaviorTreePaused)
	{
		return;
	}
	StopMovement();

	if (UBrainComponent* Brain = GetBrainComponent())
	{
		//Brain->ResumeLogic(TEXT("Return to patrol"));
		Brain->RestartLogic();
	}

	bBehaviorTreePaused = false;
}

void AEnemySoldierAIController::ForgetTarget()
{
	AActor* TargetToForget = CurrentTarget.Get();
	CurrentTarget.Reset();
	bHasLineOfSight = false;
	bInAttackRange = false;
	bKeepAimWhilePlayerInRange = false;
	LostSightElapsed = 0.0f;
	
	// Pulisce il Blackboard mentre il BT e ancora sospeso.
	// In questo modo al Resume non puo partire per un frame il vecchio ramo Combat
	WriteBlackboardState();
	SetCombatState(EEnemyCombatState::Patrol);

	// Pulisce anche la memoria della Perception: rientrando sotto i 12 m il
	// Player genera un nuovo evento di acquisizione senza dover prima arrivare a 14 m
	if (AIPerception && IsValid(TargetToForget))
	{
		AIPerception->ForgetActor(TargetToForget);
	}

	UE_LOG(LogTemp, Log, TEXT("ENEMY: bersaglio dimenticato, ritorno alla pattuglia."));
}

void AEnemySoldierAIController::WriteBlackboardState()
{
	UBlackboardComponent* BlackboardCamp = GetBlackboardComponent();
	if (!BlackboardCamp)
	{
		return;
	}
	//BlackboardCamp->SetValueAsVector(TEXT("HomeTowerLocation"), HomeLocation);

	if (AActor* TargetActor = CurrentTarget.Get())
	{
		BlackboardCamp->SetValueAsObject(TargetActorKeyName, TargetActor);
	}
	else
	{
		BlackboardCamp->ClearValue(TargetActorKeyName);
	}

	BlackboardCamp->SetValueAsBool(InAttackRangeKeyName, bInAttackRange);
}

bool AEnemySoldierAIController::IsCurrentTargetValid() const
{
	AActor* TargetActor = CurrentTarget.Get();
	if (!IsValid(TargetActor) || TargetActor->IsActorBeingDestroyed())
	{
		return false;
	}

	if (const APlayerControl* Player = Cast<APlayerControl>(TargetActor))
	{
		return !Player->IsDead();
	}

	return true;
}

bool AEnemySoldierAIController::MustAbandonTargetForSpecialeRules(AEnemySoldier* Soldier, AActor* TargetActor)const
{
	return false;
}
bool AEnemySoldierAIController::ShouldKeepAimingForSpecialeRules() const  // per dire setAiming(false)
{
	return false;
}


