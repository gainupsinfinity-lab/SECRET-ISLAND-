// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimIstancePlayerOne.generated.h"

class APlayerControl;

UCLASS()
class FANTASYPROJECT_API UAnimIstancePlayerOne : public UAnimInstance
{
	GENERATED_BODY()
protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	AActor* Actor;
	APlayerControl* PlayerControl;

	UPROPERTY(BlueprintReadOnly, Category = "Conditions")
		float Speed;
	UPROPERTY(BlueprintReadOnly, Category = "Movment")
		float Direction=0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Conditions")
		float IWR;
	UPROPERTY(BlueprintReadOnly, Category = "Conditions")
		bool isInAir;
	UPROPERTY(BlueprintReadOnly, Category = "Conditions")
		float VerticalVelocity;
	UPROPERTY(BlueprintReadOnly, Category = "Conditions")
		bool isJumping;
	UPROPERTY(BlueprintReadOnly, Category = "Conditions")
		bool isForwardJump;
	UPROPERTY(BlueprintReadOnly, Category = "Aim")
		bool bIsAiming =false;
	UPROPERTY(BlueprintReadOnly, Category = "Aim")
		float AimPitch=0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Aim")
		float AimYaw = 0.0f;
	
};
