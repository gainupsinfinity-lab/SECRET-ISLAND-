// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemySoldierAIController.generated.h"

class AEnemySoldier;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UBehaviorTree;

UENUM(BlueprintType)
enum class EEnemyCombatState : uint8  
{
	Patrol,			// Pattuglia
	Chase,			//insegue        (22-27)
	MovingFire,     //avanza e spara (6-22m )
	Attack,			//ferma e spara  (0-6m)
	Investigate     // va verso ultima pos conosciuta
};

UCLASS()
class FANTASYPROJECT_API AEnemySoldierAIController : public AAIController  // Classe Controller AI
{
	GENERATED_BODY()

public:
	AEnemySoldierAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void Tick(float DeltaSeconds) override;

	/** Il BT viene usato per la pattuglia quando non esiste un bersaglio. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy AI")
	UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy AI|Perception")
	UAIPerceptionComponent* AIPerception;

	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;

	// Distanza esatta sotto la quale il nemico si ferma e continua a sparare (6 m)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy AI|Combat", meta = (ClampMin = "100.0"))
		float AttackEnterDistance;

	//Mantenuta uguale ad AttackEnterDistance: oltre 6 m il nemico riprende subito ad avanzare
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy AI|Combat")
		float AttackExitDistance;

	//Distanza Entro il quale il nemico avanza lentamente e spara 22m
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy AI|Combat", meta = (ClampMin = "100.0"))
		float MovingFireStartDistance;

	// Oltre 27 m il bersaglio viene abbandonato immediatamente
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy AI|Combat", meta = (ClampMin = "100.0"))
		float MaxEngagmentDistance;
	//Distanza vede il player prima volta 25m
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy AI|Combat", meta = (ClampMin = "100.0"))
		float SightDistance;
	// distana oltre iluqale percezione persa 27m
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy AI|Combat", meta = (ClampMin = "100.0"))
		float LoseSightDistance;
	// cono a 85°
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy AI|Combat", meta = (ClampMin = "1.0",ClampMax="180"))
		float PeripheralVisionAngleDegrees;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy AI|Combat", meta = (ClampMin = "1.0"))
		float MoveAcceptanceRadius;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy AI|Combat", meta = (ClampMin = "1.0"))
		float CombatMoveAcceptRadius;  //margine di navigazione

	//tempo concesso prima di perdere il player dopo averlo visto
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy AI|Combat", meta = (ClampMin = "0.0"))
		float LostSightGraceTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy AI|Combat", meta = (ClampMin = "1.0", ClampMax = "45.0"))
		float AimToleranceDegrees;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy AI|Combat", meta = (ClampMin = "1.0"))
		float AimRotationInterpSpeed;

	//tempo tra l'ingresso nell'igresso stato attaccoe primo colpo
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy AI|Combat", meta = (ClampMin = "0.0"))
		float FirstShotDelay;  

	

	
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy AI|Blackboard")
		FName TargetActorKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy AI|Blackboard")
		FName InAttackRangeKeyName;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy AI|State")
		EEnemyCombatState CombatState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy AI|Territory", meta = (ClampMin = "100.0"))
		float MaxHomeDistance;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy AI|Territory")
		bool bUseHomeLimit;

	UFUNCTION()
		void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	virtual bool MustAbandonTargetForSpecialeRules(AEnemySoldier* Soldier, AActor* TargetActorr)const; //regolePer EnemyTower
	FVector HomeLocation;

	virtual bool ShouldKeepAimingForSpecialeRules() const;

private:
	void UpdateCombat(float DeltaSeconds);
	void SetCombatState(EEnemyCombatState NewState);
	void UpdateChase(AEnemySoldier* Soldier, AActor* TargetActor);
	void UpdateAttack(float DeltaSeconds);
	void UpdateInvestigate(AEnemySoldier* Soldier);
	void FaceTarget(AActor* TargetActor, float DeltaSeconds);
	void ApplyRequiredCombatTuning();
	void PauseBehaviorTreeForCombat();
	void ResumeBehaviorTreeAfterCombat();
	void ForgetTarget();
	void WriteBlackboardState();
	bool IsCurrentTargetValid() const;

	TWeakObjectPtr<AActor> CurrentTarget;
	//FVector HomeLocation;
	FVector LastKnownTargetLocation;
	float LostSightElapsed;
	float AttackStateElapsed;
	bool bHasLineOfSight;
	bool bInAttackRange;
	bool bBehaviorTreePaused;
	//bool bKeepAimUnitlPlayerDead;
	bool bKeepAimWhilePlayerInRange;
};
