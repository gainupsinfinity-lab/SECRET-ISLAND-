// Fill out your copyright notice in the Description page of Project Settings.


#include "PistolProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APistolProjectile::APistolProjectile()
{
	// Non utilizziamo Tick perché il movimento viene gestito
	   // da UProjectileMovementComponent.
	PrimaryActorTick.bCanEverTick = false;

	// ---------------------------------------------------------
	// 1. COLLISIONE
	// ---------------------------------------------------------

	CollisionComponent =CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));

	// La sfera diventa il componente principale dell'Actor
	SetRootComponent(CollisionComponent);

	// Raggio della collisione espresso in centimetri
	CollisionComponent->InitSphereRadius(3.0f);

	// Il proiettile deve bloccare gli oggetti incontrati
	CollisionComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	// Abilita la generazione dell'evento OnComponentHit.
	CollisionComponent->SetNotifyRigidBodyCollision(true);

	// Impedisce al personaggio di salire sopra il proiettile
	CollisionComponent->CanCharacterStepUpOn = ECB_No;

	// ---------------------------------------------------------
	// 2. MESH VISIBILE
	// ---------------------------------------------------------

	ProjectileMesh =CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));

	ProjectileMesh->SetupAttachment(CollisionComponent);

	// La collisione viene gestita solamente dalla sfera
	// La mesh deve essere esclusivamente visiva
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// ---------------------------------------------------------
	// 3. MOVIMENTO
	// ---------------------------------------------------------

	ProjectileMovement =CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	// Dice al componente quale oggetto deve spostare
	ProjectileMovement->UpdatedComponent =CollisionComponent;

	// 6000 cm/s equivalgono a 60 metri al secondo
	ProjectileMovement->InitialSpeed = 6000.0f;
	ProjectileMovement->MaxSpeed = 6000.0f;

	// Il proiettile ruota seguendo la sua traiettoria
	ProjectileMovement->bRotationFollowsVelocity = true;

	// Non deve rimbalzare.
	ProjectileMovement->bShouldBounce = false;

	// Nessuna caduta iniziale: traiettoria rettilinea
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	// Migliora il controllo delle collisioni ad alta velocità
	ProjectileMovement->bForceSubStepping = true;

	// ---------------------------------------------------------
	// 4. VALORI INIZIALI
	// ---------------------------------------------------------

	Damage = 20.0f;
	LifeSeconds = 5.0f;

}


void APistolProjectile::BeginPlay()
{
	Super::BeginPlay();
	// Collega l'evento di collisione alla nostra funzione
	CollisionComponent->OnComponentHit.AddDynamic(this,&APistolProjectile::OnProjectileHit);

	// Se non colpisce nulla, il proiettile viene eliminato
	// automaticamente dopo LifeSeconds
	SetLifeSpan(LifeSeconds);

	// Ignora la pistola che lo ha generato
	if (AActor* WeaponOwner = GetOwner())
	{
		CollisionComponent->IgnoreActorWhenMoving(WeaponOwner,true);
	}

	// Ignora il player che sta usando la pistola
	if (APawn* InstigatorPawn = GetInstigator())
	{
		CollisionComponent->IgnoreActorWhenMoving(InstigatorPawn,true);
	}

	
}
void APistolProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent,AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,FVector NormalImpulse,const FHitResult& Hit)
{
	// Controlla che l'Actor colpito sia valido e che non sia
	// - il proiettile stesso
	// - la pistola
	// - il player che ha sparato
	if (IsValid(OtherActor) && OtherActor != this && OtherActor != GetOwner() && OtherActor != GetInstigator())
	{
		// Direzione corrente del proiettile al momento dell'impatto
		const FVector ShotDirection =GetVelocity().GetSafeNormal();

		// Invia un evento di danno puntuale all'Actor colpito.
		UGameplayStatics::ApplyPointDamage(OtherActor,Damage,ShotDirection,Hit,GetInstigatorController(),this,UDamageType::StaticClass());
	}

	// Dopo qualsiasi impatto il proiettile viene eliminato
	Destroy();
}


