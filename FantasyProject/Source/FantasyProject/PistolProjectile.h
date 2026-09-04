// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PistolProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UPrimitiveComponent;

UCLASS()
class FANTASYPROJECT_API APistolProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	
	APistolProjectile();

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Projectile|Components")
		USphereComponent* CollisionComponent;

	// Parte visibile del proiettile.
	// La Static Mesh verrˆ scelta in BP_PistolProjectile
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Projectile|Components")
		UStaticMeshComponent* ProjectileMesh;

	// Componente Unreal che sposta automaticamente il proiettile
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Projectile|Movement")
		UProjectileMovementComponent* ProjectileMovement;

	// Quantitˆ di danno causata dall'impatto.
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Projectile|Damage",meta = (ClampMin = "0.0"))
		float Damage;

	// Numero massimo di secondi di vita del proiettile.
	// Serve a eliminare i proiettili che non colpiscono nulla
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Projectile|Lifetime",meta = (ClampMin = "0.1"))
		float LifeSeconds;

	// Funzione chiamata automaticamente da Unreal
	// quando CollisionComponent colpisce qualcosa
	UFUNCTION()
		void OnProjectileHit(UPrimitiveComponent* HitComponent,AActor* OtherActor,UPrimitiveComponent* OtherComponent,FVector NormalImpulse,const FHitResult& Hit);


};
