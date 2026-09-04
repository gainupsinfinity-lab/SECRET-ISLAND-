// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MineFieldProgressGate.generated.h"

class APlayerControl;
class UBoxComponent;
class USceneComponent;

UCLASS()
class FANTASYPROJECT_API AMineFieldProgressGate : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AMineFieldProgressGate();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minefield|Components")
		USceneComponent* Root;

	// Da collocare subito dopo l'ultima mina.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minefield|Components")
		UBoxComponent* MinefieldExitTrigger;  // rileva il player quando termina il campo minato

	// Da collocare davanti alla barriera per mostrare cosa manca al player.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minefield|Components")
		UBoxComponent* RequirementTrigger; //rileva quando si avvicina alla barriera senza borsa

	// Collisione invisibile che impedisce di oltrepassare la strada.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minefield|Components")
		UBoxComponent* RoadBarrier; //blocca il player fisicamente

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minefield|Requirements")
		bool bRequireMinefieldCompletion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minefield|UI")
		FText MinefieldCompletedText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minefield|UI", meta = (ClampMin = "0.1"))
		float MissionMessageDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minefield|Checks", meta = (ClampMin = "0.05"))
		float RequirementCheckInterval;

	UFUNCTION()
		void OnMinefieldExitBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,
			const FHitResult& SweepResult
		);  //componte overlap MinefieldExiTrigger/Actor player /altro Axctor Capsule /int32 per corpi fisici/BfromSweep se è stato rilevato overlap/Fhit sullimpatto

	UFUNCTION()
		void OnRequirementBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,
			const FHitResult& SweepResult  //componte overlap rEQUIREMNET..Fine se player è vivo e inoltra il messaggio
		);

private:
	void CheckUnlockRequirements(); // controllo borsa 
	void UnlockRoad();
	void ShowMissingRequirements(APlayerControl* Player) const;
	APlayerControl* ResolvePlayer() const;

	FTimerHandle RequirementCheckTimerHandle;
	bool bMinefieldCompleted;
	bool bRoadUnlocked;

};
