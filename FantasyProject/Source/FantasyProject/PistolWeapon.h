// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PistolWeapon.generated.h"

class UArrowComponent;
class APistolProjectile;
class USoundBase;

UCLASS()
class FANTASYPROJECT_API APistolWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	
	APistolWeapon();

	UFUNCTION(BlueprintPure, Category = "Weapon")
		UArrowComponent* GetMuzzlePoint() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon|Projectile")
		bool FireAtTarget(const FVector& TargetLocation);
	

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon")
		UArrowComponent* CacheMuzzlePoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Projectile")
		TSubclassOf< APistolProjectile>ProjectileClass; // contiene classe per creare proiettile

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Sound")
		USoundBase* FireSound = nullptr;

};

