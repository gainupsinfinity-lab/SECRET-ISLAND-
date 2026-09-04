// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThrownGrenade.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;
class USoundBase;
class UPrimitiveComponent;

UCLASS()
class FANTASYPROJECT_API AThrownGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThrownGrenade();
    void LaunchGrenade(const FVector& Direction);

protected:
    virtual void BeginPlay() override;

    // COLLISIONE
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grenade|Components")
        USphereComponent* CollisionComponent;

    // 4 PARTI DELLA GRANATA
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grenade|Components")
        UStaticMeshComponent* GrenadeBase;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grenade|Components")
        UStaticMeshComponent* GrenadeBracket;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grenade|Components")
        UStaticMeshComponent* GrenadeCircle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grenade|Components")
        UStaticMeshComponent* GrenadeUp;

    // MOVIMENTO
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grenade|Movement")
        UProjectileMovementComponent* GrenadeMovement;

    // DANNO MASSIMO VICINO ALL'ESPLOSIONE
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade|Damage")
        float MaxDamage;

    // DANNO MINIMO AL LIMITE DELL'ESPLOSIONE
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade|Damage")
        float MinDamage;

    // ENTRO QUESTO RAGGIO DANNO MASSIMO
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade|Damage")
        float InnerDamageRadius;

    // OLTRE QUESTO RAGGIO NESSUN DANNO
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade|Damage")
        float OuterDamageRadius;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade|Damage")
        float DamageFalloff;

    // EFFETTI
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade|Effects")
        UNiagaraSystem* ExplosionEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade|Effects")
        UNiagaraSystem* FireEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade|Effects")
        USoundBase* ExplosionSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade|Lifetime")
        float LifeSeconds;

private:
    UFUNCTION()
        void OnGrenadeHit(UPrimitiveComponent* HitComponent,AActor* OtherActor,UPrimitiveComponent* OtherComponent,FVector NormalImpulse,const FHitResult& Hit);

    void Explode();

    bool bExploded;

};
