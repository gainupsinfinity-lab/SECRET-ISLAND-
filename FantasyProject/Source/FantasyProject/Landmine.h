// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Landmine.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class USphereComponent;
class UPrimitiveComponent;
class UNiagaraSystem;
class USoundBase;
class APlayerControl;


UCLASS()
class FANTASYPROJECT_API ALandmine : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ALandmine();

protected:
	
	virtual void BeginPlay() override;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Landmine|Components")
        USceneComponent* SceneRoot;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Landmine|Components")
        UStaticMeshComponent* MineMesh;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Landmine|Components")
        USphereComponent* TriggerSphere;

    // Raggio massimo entro il quale la mina può causare danni.
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Landmine|Damage",meta = (ClampMin = "1.0"))
        float OuterDamageRadius;

    // Entro questo raggio il colpo è letale.
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Landmine|Damage",
        meta = (ClampMin = "0.0"))
        float LethalRadius;

    // Margine aggiuntivo della sfera che avvia i controlli.
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Landmine|Detection",meta = (ClampMin = "0.0"))
        float TriggerPadding;

    // Evita che un piede molto più in alto attivi la mina.
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Landmine|Detection",meta = (ClampMin = "0.0"))
        float MaxFootHeightDifference;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Landmine|Damage",meta = (ClampMin = "0.0", ClampMax = "100.0"))
        float DamagePercent;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Landmine|Detection",meta = (ClampMin = "0.0"))
        float InitialEvaluationDelay;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Landmine|Detection",meta = (ClampMin = "0.01"))
        float EvaluationInterval;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Landmine|Effects",meta = (ClampMin = "0.1"))
        float DestroyDelay;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Landmine|Effects")
        UNiagaraSystem* ExplosionEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landmine|Effects")
        UNiagaraSystem* SandExplosionEffect;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Landmine|Effects")
        USoundBase* ExplosionSound;




private:

    UFUNCTION()
        void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,
            const FHitResult& SweepResult); //rileva piede in entrata

    UFUNCTION()
        void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);  //piede in uscita

    void EvaluatePendingFoot();

    void Explode(APlayerControl* Player,bool bLethalHit); //gestione esplosione

    void ClearPendingFoot();

    TWeakObjectPtr<APlayerControl> PendingPlayer;

    TWeakObjectPtr<UPrimitiveComponent> PendingFootComponent;

    FTimerHandle EvaluationTimerHandle; //intervallo di tempo

    bool bExploded;


};
