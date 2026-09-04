// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AmmoComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged,int32, CurrentAmmo,int32, MaxAmmo);  // MACRO PER EVENT 2 Valori CurrentAmmo Max Ammo  


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FANTASYPROJECT_API UAmmoComponent : public UActorComponent // classe posseduta da APlayer
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAmmoComponent();

	UFUNCTION(BlueprintPure, Category = "Weapon|Ammo")
		bool CanFire() const; // controllo munzioni se can fire

	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
		bool ConsumeRound(); //controllo consumo colpo

	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo") //call da bluporint
		int32 AddAmmo(int32 Amount); // Add munizioni

	UFUNCTION(BlueprintPure, Category = "Weapon|Ammo")
		bool IsFull() const;  //controlla se è caricato

	UFUNCTION(BlueprintPure, Category = "Weapon|Ammo")
		int32 GetCurrentAmmo() const; // lettura valori 

	UFUNCTION(BlueprintPure, Category = "Weapon|Ammo")
		int32 GetMaxAmmo() const;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Ammo")
		FOnAmmoChanged OnAmmoChanged;  // evento e collegarlo al bluprint

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Weapon|Ammo",meta = (ClampMin = "1")) //no config <1
		int32 MaxAmmo;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Weapon|Ammo",meta = (ClampMin = "0"))
		int32 InitialAmmo;// n numunizioni inziali

	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category = "Weapon|Ammo")
		int32 CurrentAmmo; // numero munizioni correnti

private:
	void BroadcastAmmoChanged();

		
};
