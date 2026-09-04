// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBullet.h"
#include "EnemySoldier.generated.h"

class UAnimMontage;
class UArrowComponent;
class UNiagaraSystem;
class USoundBase;
class AEnemyBullet;
class AItemPickup;

UCLASS()
class FANTASYPROJECT_API AEnemySoldier : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemySoldier();

	virtual float TakeDamage(float DamageAmount,const FDamageEvent& DamageEvent,AController* EventInstigator,AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
	void SetAiming(bool bNewAiming);

	UFUNCTION(BlueprintPure, Category = "Enemy|Combat")
	bool IsAiming() const;

	/**
	 * Riproduce il montage di fuoco ed esegue un colpo hitscan verso il bersaglio.
	 * Restituisce true soltanto quando parte un nuovo colpo (rispetta FireInterval).
	 */
	UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
	bool FireAtTarget(AActor* TargetActor);

	/** Passa tra rotazione verso il movimento e rotazione verso il controller. */
	void SetCombatMovementMode(bool bCombatMode);

	UFUNCTION(BlueprintPure, Category = "Enemy|State")
	bool IsDead() const
	{
		return bIsDead;
	}

	UFUNCTION(BlueprintPure, Category = "Enemy|Movement")
	float GetWalkSpeed() const
	{
		return WalkSpeed;
	}

	void SetFireSoundEnable(bool bEnabled);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Stats", meta = (ClampMin = "1.0"))
		float MaxHealth;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy|Stats")
		float CurrentHealth;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy|State")
		bool bIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Movement", meta = (ClampMin = "0.0"))
		float WalkSpeed;

	/** Componente da allineare alla volata del fucile nel BP_EnemySoldier. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Weapon")
		UArrowComponent* MuzzlePoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Weapon")
		UAnimMontage* FireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Sound")
		USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Sound")
		USoundBase* HitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Sound")
		USoundBase* ImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Sound")
		USoundBase* DeathSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Weapon")
		UNiagaraSystem* MuzzleFlashEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Weapon", meta = (ClampMin = "0.0"))
		float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Weapon", meta = (ClampMin = "100.0"))
		float WeaponRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Weapon", meta = (ClampMin = "0.05"))
		float FireInterval;

	/** Altezza aggiunta alla posizione dell'Actor per mirare al busto. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Weapon")
		float TargetAimHeightOffset;

	// Dispersione minima dell'arma nemica.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Weapon",meta = (ClampMin = "0.0", ClampMax = "15.0"))
		float BaseSpreadDegrees;

	// Dispersione aggiuntiva quando il bersaglio si muove.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Weapon",meta = (ClampMin = "0.0", ClampMax = "15.0"))
		float MovingTargetSpreadBonusDegrees;
		

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Debug")
		bool bDrawDebugShot;

	UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
		void Die();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Weapon")
		TSubclassOf<AEnemyBullet>EnemyBulletClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Damage")
		UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Damage")
		UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Damage", meta = (ClampMin = "1"))
		int32 HitsBeforeDeath; //colpi rivuti dal player per die

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy|Damage")
		int32 HitsTaken;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Damage", meta = (ClampMin = "0.0"))
		float CorpseLifeSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Loot")
		bool bDropKeyOnDeath;  //verifica chi posside la chiave

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Loot", meta = (EditCondition = "bDropKeyOnDeath"))
		TSubclassOf<AItemPickup>KeyPickupClass; //classe da generare la chiave

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Loot", meta = (EditCondition = "bDropKeyOnDeath"))
		FVector KeyDropOffset;  // stampa genera key

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Loot", meta = (EditCondition = "bDropKeyOnDeath"))
		USoundBase* KeyDropSound;

	void DropKeyOnDeath();



private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy|Combat", meta = (AllowPrivateAccess = "true"))
		bool bIsAiming;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy|Damage", meta = (AllowPrivateAccess = "true"))
		bool bIsReacting;

	FTimerHandle HitReactTimeHandle;
	void EndHitReaction();
	float LastFireTime;
	bool bFireSoundEnable;
};
