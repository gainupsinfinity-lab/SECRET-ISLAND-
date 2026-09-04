// Fill out your copyright notice in the Description page of Project Settings.


#include "PistolWeapon.h"
#include "Components/ArrowComponent.h"
#include "Engine/Engine.h"
#include "PistolProjectile.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "GameFramework/Pawn.h"



APistolWeapon::APistolWeapon()
{
 	
	PrimaryActorTick.bCanEverTick = false;
	CacheMuzzlePoint = nullptr;

}


void APistolWeapon::BeginPlay()
{
	Super::BeginPlay();
	CacheMuzzlePoint = FindComponentByClass<UArrowComponent>();  // memorizza una riferimento per riutilizzarlo

	if (!CacheMuzzlePoint)
	{
		UE_LOG(LogTemp,Error,TEXT("PistolWeapon: MuzzlePoint non trovato nel Blueprint."));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("PistolWeapon: MuzzlePoint trovato %s."), *CacheMuzzlePoint->GetName());
	}
	
	
}

	

UArrowComponent* APistolWeapon::GetMuzzlePoint() const  // verifica dove si trova
{
	return CacheMuzzlePoint;
}

bool APistolWeapon::FireAtTarget(const FVector& TargetLocation)  // riceve punto 3d nel wolrd
{
	// La freccia MuzzlePoint deve esistere.
	if (!IsValid(CacheMuzzlePoint))
	{
		UE_LOG(LogTemp,Error,TEXT("PistolWeapon: MuzzlePoint non valido."));

		return false;
	}

	// Nel Blueprint della pistola deve essere assegnata
	// la classe BP_PistolProjectile.
	if (!ProjectileClass)
	{
		UE_LOG(LogTemp,Error,TEXT("PistolWeapon: ProjectileClass non assegnata."));

		return false;
	}

	UWorld* World = GetWorld();  //per lo spawn serve il mondo unrealper creare l'Actor

	if (!World)
	{
		return false;
	}

	// Posizione esatta dell'imboccatura della canna.
	const FVector SpawnLocation =CacheMuzzlePoint->GetComponentLocation();

	// Vettore che va dalla canna al bersaglio.
	const FVector ShotDirection =(TargetLocation - SpawnLocation).GetSafeNormal();  // vettore =dove volgio arrivare  - doveda dove spara il colpo  a lenght 1

	// Impedisce di utilizzare una direzione nulla.
	if (ShotDirection.IsNearlyZero())
	{
		return false;
	}

	// Converte la direzione in una rotazione Unreal
	const FRotator SpawnRotation =ShotDirection.Rotation();

	FActorSpawnParameters SpawnParameters;

	// La pistola diventa Owner del proiettile.
	SpawnParameters.Owner = this;

	// Il player che possiede la pistola diventa
	// Instigator del proiettile.
	SpawnParameters.Instigator =Cast<APawn>(GetOwner());  // Pawn responsabile dell'azione 

	// Permette la creazione anche se il MuzzlePoint  
	// molto vicino alla mano o alla mesh del personaggio.
	SpawnParameters.SpawnCollisionHandlingOverride =ESpawnActorCollisionHandlingMethod::AlwaysSpawn;  // crea projecyile anche vicino/dentro collisione percheè muzzle pint vicino

	APistolProjectile* SpawnedProjectile = World->SpawnActor<APistolProjectile>(ProjectileClass,SpawnLocation,SpawnRotation,SpawnParameters);  // creazione projectile

	if (!IsValid(SpawnedProjectile))
	{
		UE_LOG(LogTemp,Error,TEXT("FireAttARGET: creazione del proiettile fallita."));
		return false;
	}
	if (FireSound) 
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, SpawnLocation);
	}
	UE_LOG(LogTemp, Warning, TEXT("FIREATTARGET: proiettile %s creato da %s verso %s."), *GetNameSafe(SpawnedProjectile), *SpawnLocation.ToString(), *TargetLocation.ToString());
	return true;
		
	// colpo consumato solo quando restituisce true
}



