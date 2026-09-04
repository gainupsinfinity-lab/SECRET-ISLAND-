// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrownGrenade.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"


AThrownGrenade::AThrownGrenade()
{
 	
    PrimaryActorTick.bCanEverTick = false;

    // ----------------------------------------------------
    // COLLISIONE
    // ----------------------------------------------------

    CollisionComponent =CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));

    SetRootComponent(CollisionComponent);

    CollisionComponent->InitSphereRadius(8.0f);

    CollisionComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));

    CollisionComponent->SetNotifyRigidBodyCollision(true);

    CollisionComponent->CanCharacterStepUpOn = ECB_No;

    // ----------------------------------------------------
    // MESH
    // ----------------------------------------------------

    GrenadeBase =CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeBase"));

    GrenadeBase->SetupAttachment(CollisionComponent);
    GrenadeBase->SetCollisionEnabled(ECollisionEnabled::NoCollision);


    GrenadeBracket =CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeBracket"));

    GrenadeBracket->SetupAttachment(GrenadeBase);
    GrenadeBracket->SetCollisionEnabled(ECollisionEnabled::NoCollision);


    GrenadeCircle =CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeCircle"));

    GrenadeCircle->SetupAttachment(GrenadeBase);
    GrenadeCircle->SetCollisionEnabled(ECollisionEnabled::NoCollision);


    GrenadeUp =CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeUp"));

    GrenadeUp->SetupAttachment(GrenadeBase);
    GrenadeUp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // ----------------------------------------------------
    // MOVIMENTO
    // ----------------------------------------------------

    GrenadeMovement =CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

    GrenadeMovement->UpdatedComponent = CollisionComponent;

    GrenadeMovement->InitialSpeed = 1300.0f;
    GrenadeMovement->MaxSpeed = 1300.0f;

    GrenadeMovement->bRotationFollowsVelocity = true;

    // Appena collide esplode, quindi niente rimbalzo.
    GrenadeMovement->bShouldBounce = false;

    // La granata deve fare una parabola.
    GrenadeMovement->ProjectileGravityScale = 1.0f;

    GrenadeMovement->bForceSubStepping = true;

    GrenadeMovement->bInitialVelocityInLocalSpace = false;

    // ----------------------------------------------------
    // DANNO
    // ----------------------------------------------------

    MaxDamage = 100.0f;
    MinDamage = 15.0f;

    InnerDamageRadius = 300.0f;   // 1,5 metri
    OuterDamageRadius = 800.0f;   // 8 metri

    DamageFalloff = 1.0f;

    // ----------------------------------------------------

    ExplosionEffect = nullptr;
    FireEffect = nullptr;
    ExplosionSound = nullptr;

    LifeSeconds = 8.0f;

    bExploded = false;
}
void AThrownGrenade::BeginPlay()
{
    Super::BeginPlay();

    CollisionComponent->OnComponentHit.AddDynamic(
        this,
        &AThrownGrenade::OnGrenadeHit
    );

    SetLifeSpan(LifeSeconds);

    // Non deve esplodere contro il Player che l'ha lanciata.
    if (APawn* InstigatorPawn = GetInstigator())
    {
        CollisionComponent->IgnoreActorWhenMoving(
            InstigatorPawn,
            true
        );
    }
}
void AThrownGrenade::OnGrenadeHit(UPrimitiveComponent* HitComponent,AActor* OtherActor,UPrimitiveComponent* OtherComponent,FVector NormalImpulse,const FHitResult& Hit)
{
    if (bExploded)
    {
        return;
    }

    // Ignora il Player che l'ha lanciata.
    if (OtherActor == GetInstigator())
    {
        return;
    }
    if (IsValid(OtherActor) && OtherActor->ActorHasTag(TEXT("FammableHouse")))
    {
        if (FireEffect && GetWorld())
        {
            const FVector FireLocation = Hit.ImpactPoint + Hit.ImpactNormal * 2.0f; // sposta il fuoco dalla superficie per evitare dentro il muro
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FireEffect, FireLocation, FRotator::ZeroRotator,FVector(0.8f));
        }
      
    }

    Explode();
}
void AThrownGrenade::Explode()
{
    if (bExploded)
    {
        return;
    }

    bExploded = true;

    const FVector ExplosionLocation = GetActorLocation();

    // Blocco nuove collisioni.
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision
    );

    // -----------------------------
    // NIAGARA
    // -----------------------------

    if (ExplosionEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),ExplosionEffect,ExplosionLocation,GetActorRotation());
    }

    // -----------------------------
    // SUONO
    // -----------------------------

    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this,ExplosionSound,ExplosionLocation);
    }

    // -----------------------------
    // DANNO RADIALE CON FALLOFF
    // -----------------------------

    TArray<AActor*> IgnoreActors;

    // Per ora evitiamo che il Player uccida sé stesso.
    if (AActor* InstigatorActor = GetInstigator())
    {
        IgnoreActors.Add(InstigatorActor);
    }

    UGameplayStatics::ApplyRadialDamageWithFalloff(
        this,
        MaxDamage,              // 100
        MinDamage,              // 15
        ExplosionLocation,
        InnerDamageRadius,      // 1,5 m
        OuterDamageRadius,      // 5 m
        DamageFalloff,
        UDamageType::StaticClass(),
        IgnoreActors,
        this,
        GetInstigatorController()
    );

    // Nasconde tutta la granata.
    GrenadeBase->SetVisibility(false, true);

    Destroy();
}

void AThrownGrenade::LaunchGrenade(const FVector& Direction)
{
    if (!GrenadeMovement)
    {
        return;
    }
    //mov direzione actor/granata    //tranf vettore normal lenght 1
    GrenadeMovement->Velocity = Direction.GetSafeNormal() * GrenadeMovement->InitialSpeed; //dir/vel/mov
}




