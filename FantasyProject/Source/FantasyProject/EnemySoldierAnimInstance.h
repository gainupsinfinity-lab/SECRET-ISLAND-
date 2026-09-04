// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemySoldierAnimInstance.generated.h"

class AEnemySoldier;

// parametri usati dall'Animation Blueprint del soldato.

UCLASS(Transient, Blueprintable)
class FANTASYPROJECT_API UEnemySoldierAnimInstance : public UAnimInstance  // classe che gestisce i parametri dell'Animation BP
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;  //Iinizializzazione Anim
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;  // Update

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Enemy|Locomotion")
	float Speed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy|Locomotion")
	float Direction = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy|Locomotion")
	bool bShouldMove = false;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy|Locomotion")
	bool bIsInAir = false;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy|Combat")
	bool bIsAiming = false;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy|Combat")
	float AimYaw = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy|Combat")
	float AimPitch = 0.0f;

private:
	TWeakObjectPtr<AEnemySoldier> SoldierOwner;  //weak pointer conserva un riferimento al soldier .Se esiste Twaj legge se distrutto non valido
};
