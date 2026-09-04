// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyBullet.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UPrimitiveComponent;

UCLASS()
class FANTASYPROJECT_API AEnemyBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AEnemyBullet();
	void SetDamage(float InDamage);

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet")
		USphereComponent* Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet")
		UStaticMeshComponent* BulletMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet")
		UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bullet",
		meta = (ClampMin = "0.0"))
		float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bullet",
		meta = (ClampMin = "100.0"))
		float Speed;

	UFUNCTION()
		void HandleHit(UPrimitiveComponent* HitComponent,AActor* OtherActor,UPrimitiveComponent* OtherComponent,FVector NormalImpulse,const FHitResult& Hit);

};
