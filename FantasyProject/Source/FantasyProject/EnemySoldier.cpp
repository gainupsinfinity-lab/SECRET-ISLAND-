// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySoldier.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "EnemySoldierAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Animation/AnimMontage.h"
#include "Engine/World.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "ThrownGrenade.h"
#include "Components/SkeletalMeshComponent.h"
#include "PlayerControl.h"
#include "ItemPickup.h"

AEnemySoldier::AEnemySoldier(): 
	MaxHealth(100.0f),
	CurrentHealth(100.0f),
	bIsDead(false),
	WalkSpeed(220.0f),
	MuzzlePoint(nullptr),
	FireMontage(nullptr),
	FireSound(nullptr),
	HitSound(nullptr),
	ImpactSound(nullptr),
	DeathSound(nullptr),
	MuzzleFlashEffect(nullptr),
	AttackDamage(10.0f),
	WeaponRange(5000.0f),
	FireInterval(2.3f),
	TargetAimHeightOffset(45.0f),
	BaseSpreadDegrees(2.0f),
	MovingTargetSpreadBonusDegrees(3.0f),
	bDrawDebugShot(false),
	HitReactMontage(nullptr),
	DeathMontage(nullptr),
	HitsBeforeDeath(5),
	HitsTaken(0),
	CorpseLifeSeconds(5.0f),
	bDropKeyOnDeath(false),
	KeyPickupClass(nullptr),
	KeyDropOffset(0.0f,0.0f,-70.0f),
	KeyDropSound(nullptr),
	bIsAiming(false),
	bIsReacting(false),
	LastFireTime(-1000.0f),
	bFireSoundEnable(true)


{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	AIControllerClass = AEnemySoldierAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true;
	Movement->bUseControllerDesiredRotation = true;
	Movement->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	Movement->MaxWalkSpeed = WalkSpeed;
	Movement->MaxAcceleration = 900.0f;
	Movement->BrakingDecelerationWalking = 1200.0f;
	Movement->bRequestedMoveUseAcceleration = true;

	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// Il componente compare nel Blueprint e va spostato sulla punta della canna.
	MuzzlePoint = CreateDefaultSubobject<UArrowComponent>(TEXT("MuzzlePoint"));
	MuzzlePoint->SetupAttachment(GetMesh(), FName(TEXT("WeaponSocket")));
	MuzzlePoint->SetRelativeLocation(FVector(55.0f, 0.0f, 0.0f));
	MuzzlePoint->SetHiddenInGame(true);
}

void AEnemySoldier::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

float AEnemySoldier::TakeDamage(float DamageAmount,const FDamageEvent& DamageEvent,AController* EventInstigator,AActor* DamageCauser)
{
	if (bIsDead || DamageAmount <= 0.0f)
	{
		return 0.0f;
	}


	// Applica il normale sistema di danno Unreal.
	const float AppliedDamage =
		Super::TakeDamage(DamageAmount,DamageEvent,EventInstigator,DamageCauser);


	// Utilizziamo il danno realmente ricevuto.
	const float FinalDamage =AppliedDamage > 0.0f? AppliedDamage: DamageAmount;


	// ------------------------------------------------
	// TOGLIE VITA
	// ------------------------------------------------

	CurrentHealth =FMath::Clamp(CurrentHealth - FinalDamage,0.0f,MaxHealth);


	// ------------------------------------------------
	// CONTROLLO GRANATA
	// ------------------------------------------------

	const bool bDamageFromGrenade =Cast<AThrownGrenade>(DamageCauser) != nullptr;


	// Soltanto pistola / colpi normali aumentano
	// il contatore HitsTaken.
	if (!bDamageFromGrenade)
	{
		++HitsTaken;
	}
	// ------------------------------------------------
	// DEBUG GRANATA SULLA UI
	// ------------------------------------------------

	if (bDamageFromGrenade)
	{
		if (APlayerControl* Player =Cast<APlayerControl>(UGameplayStatics::GetPlayerCharacter(this, 0)))
		{
			float GrenadeDistanceMeters = 0.0f;

			if (IsValid(DamageCauser))
			{
				GrenadeDistanceMeters =FVector::Distance(DamageCauser->GetActorLocation(),GetActorLocation()) / 100.0f;
			}


			const FString Message =FString::Printf(TEXT("GRANATA | distanza %.1f m | danno %.0f | vita %.0f/%.0f"),GrenadeDistanceMeters,FinalDamage,CurrentHealth,MaxHealth);


			Player->ShowDebugWarning(Message);
		}
	}


	UE_LOG(LogTemp,Warning,TEXT("Enemy: danno %.1f | vita %.1f | granata %s | colpi %d/%d"),FinalDamage,CurrentHealth,bDamageFromGrenade ? TEXT("SI") : TEXT("NO"),
		HitsTaken,HitsBeforeDeath);
	// ------------------------------------------------
	// MORTE
	// ------------------------------------------------

	if (CurrentHealth <= 0.0f ||(!bDamageFromGrenade &&HitsTaken >= HitsBeforeDeath))
	{
		Die();
	}
	else
	{
		if (HitReactMontage)
		{
			bIsReacting = true;

			const float MontageDuration =PlayAnimMontage(HitReactMontage);

			// Solo se il Montage parte davvero
			if (MontageDuration > 0.0f)
			{
				// Voce/lamento del nemico
				if (HitSound)
				{
					UGameplayStatics::PlaySoundAtLocation(this,HitSound,GetActorLocation());
				}

				// Suono dell'impatto del colpo
				if (ImpactSound)
				{
					UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,GetActorLocation());
				}

				GetWorldTimerManager().ClearTimer(HitReactTimeHandle);

				GetWorldTimerManager().SetTimer(HitReactTimeHandle,this,&AEnemySoldier::EndHitReaction,MontageDuration,false);
			}
			else
			{
				bIsReacting = false;

				UE_LOG(LogTemp,Warning,TEXT("HitReactMontage non riprodotto: controlla Skeleton e Slot"));
			}
		}
		else
		{
			bIsReacting = false;

			UE_LOG(LogTemp,Warning,TEXT("HitReactMontage non assegnato."));
		}
	}

	return FinalDamage;
}
/*  ALCUNI SCRIPT DI TEST 
float AEnemySoldier::TakeDamage(float DamageAmount,const FDamageEvent& DamageEvent,AController* EventInstigator,AActor* DamageCauser)
{
	if (bIsDead || DamageAmount <= 0.0f)
	{
		return 0.0f;
	}

	Super::TakeDamage(DamageAmount,DamageEvent,EventInstigator,DamageCauser);


	// ------------------------------------------------
	// CONTROLLO: IL DANNO ARRIVA DALLA GRANATA?
	// ------------------------------------------------

	bool bDamageFromGrenade = false;

	if (Cast<AThrownGrenade>(DamageCauser))
	{
		bDamageFromGrenade = true;
	}
	float FinalDamage = 0.0f;

	if (bDamageFromGrenade)
	{
		// La granata mantiene il vero danno
		// calcolato in base alla distanza.
		FinalDamage = DamageAmount;
	}
	else
	{
		// I colpi normali vengono calcolati
		// in base a HitsBeforeDeath.

		FinalDamage =MaxHealth /FMath::Max(1, HitsBeforeDeath);

		++HitsTaken;
	}
	// ------------------------------------------------
	// TOGLIE VITA
	// ------------------------------------------------

	CurrentHealth = FMath::Clamp(CurrentHealth - FinalDamage,0.0f,MaxHealth);


	UE_LOG(LogTemp,Warning,TEXT("Danno %.1f | Vita %.1f/%.1f | Grenade %s | Hits %d/%d"),FinalDamage,CurrentHealth,
		MaxHealth,bDamageFromGrenade ? TEXT("SI") : TEXT("NO"),HitsTaken,HitsBeforeDeath);
	// ------------------------------------------------
	// MORTE
	// ------------------------------------------------

	if (CurrentHealth <= 0.0f)
	{
		Die();
	}
	else
	{
		// QUI lasci il tuo HitReact già esistente.
		if (HitReactMontage)
		{
			bIsReacting = true;

			const float MontageDuration =PlayAnimMontage(HitReactMontage);

			if (MontageDuration > 0.0f)
			{
				GetWorldTimerManager().ClearTimer(HitReactTimeHandle);

				GetWorldTimerManager().SetTimer(HitReactTimeHandle,this,&AEnemySoldier::EndHitReaction,MontageDuration,false);
			}
		}
	}

	return FinalDamage;
} */
/*
float AEnemySoldier::TakeDamage(float DamageAmount,const FDamageEvent& DamageEvent,AController* EventInstigator,AActor* DamageCauser)
{
	if (bIsDead || DamageAmount <= 0.0f)
	{
		return 0.0f;
	}

	const float AppliedDamage = Super::TakeDamage(DamageAmount,DamageEvent,EventInstigator,DamageCauser);

	const float FinalDamage =AppliedDamage > 0.0f ? AppliedDamage : DamageAmount;

	CurrentHealth = FMath::Clamp(CurrentHealth - FinalDamage,0.0f,MaxHealth);
	// CONTROLLO DANNO GRANATA
	const bool bDamageFromGrenade = Cast<AThrownGrenade>(DamageCauser) != nullptr;  // VERIFICA SE IL DANNO PROVOCATO DA Actor/Granata

	if (!bDamageFromGrenade)
	{
		++HitsTaken;
	}
	

	UE_LOG(LogTemp,Warning,TEXT("Enemy colpito: danno %.1f | vita %.1f | granata %s | colpi %d/%d"),FinalDamage,CurrentHealth,
		bDamageFromGrenade ? TEXT("SI") : TEXT("NO"), HitsTaken, HitsBeforeDeath);

	if (CurrentHealth <= 0.0f || (!bDamageFromGrenade && HitsTaken >= HitsBeforeDeath))
	{
		Die();
	}
	else 
	{
		if (HitReactMontage)
		{
			bIsReacting = true;
			const float MontageDuration = PlayAnimMontage(HitReactMontage);
			if (HitSound || ImpactSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
				
			}
			if (ImpactSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
			}
			if (MontageDuration > 0.0f)
			{
				GetWorldTimerManager().ClearTimer(HitReactTimeHandle);

				GetWorldTimerManager().SetTimer(HitReactTimeHandle, this, &AEnemySoldier::EndHitReaction, MontageDuration, false);
			}
		}

		else 
		{ 
			bIsReacting = false;
			UE_LOG(LogTemp,Warning,TEXT("HitReactMontage non riprodotto: controlla Skeleton e Slot"));
		}
	}

	return FinalDamage;
}
*/


/****************/

void AEnemySoldier::SetFireSoundEnable(bool Enabled)
{
	bFireSoundEnable = Enabled;
}


void AEnemySoldier::EndHitReaction()
{
	bIsReacting = false;
}

void AEnemySoldier::Die()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;
	DropKeyOnDeath();
	bIsAiming = false;

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound,GetActorLocation());
		
	}

	if (AAIController* AIController =Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();

		if (UBrainComponent* Brain = AIController->GetBrainComponent())
		{
			Brain->StopLogic(TEXT("Enemy dead"));
		}
	}

	if (UCharacterMovementComponent* Movement =GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
		Movement->DisableMovement();
	}

	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//SetActorEnableCollision(false);

	float DeathDuration = 0.0f;

	if (DeathMontage)
	{
		DeathDuration = PlayAnimMontage(DeathMontage);
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("DeathMontage non assegnato"));
	}
	SetLifeSpan(FMath::Max(DeathDuration, 0.0f) +CorpseLifeSeconds);
}
void AEnemySoldier::DropKeyOnDeath()
{
	if (!bDropKeyOnDeath || !KeyPickupClass || !GetWorld())
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.SpawnCollisionHandlingOverride =ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AItemPickup* DroppedKey=GetWorld()->SpawnActor<AItemPickup>(KeyPickupClass,GetActorLocation() + KeyDropOffset,FRotator::ZeroRotator,SpawnParameters);

	if (DroppedKey && KeyDropSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, KeyDropSound, DroppedKey->GetActorLocation());
	}
	if (IsValid(DroppedKey))
	{
		if (APlayerControl* Player = Cast<APlayerControl>(UGameplayStatics::GetPlayerCharacter(this, 0)))
		{
			Player->ShowProgressNotification(FText::FromString(TEXT("Il Nemico ha lasciato la chiave per terra")),2.5);
		}
	}


}





void AEnemySoldier::SetAiming(bool bNewAiming)
{
	const bool bShouldAim = bNewAiming && !bIsDead;

	// 
	if (!bShouldAim && FireMontage)
	{
		if (USkeletalMeshComponent* MeshComponent = GetMesh())
		{
			if (UAnimInstance* AnimIstance = MeshComponent->GetAnimInstance())
			{
				if (AnimIstance->Montage_IsPlaying(FireMontage))
				{
					AnimIstance->Montage_Stop(0.05f, FireMontage);
				}
			}
		}
	}
	bIsAiming = bShouldAim;
}

bool AEnemySoldier::IsAiming() const
{
	return bIsAiming;
}

void AEnemySoldier::SetCombatMovementMode(bool bCombatMode)
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement)
	{
		return;
	}

	// In movimento il corpo segue la velocita. Durante l'attacco segue il controller.
	Movement->bOrientRotationToMovement = !bCombatMode;
	Movement->bUseControllerDesiredRotation = bCombatMode;
}
//**  TEST SCRIPT **//

/*bool AEnemySoldier::FireAtTarget(AActor* TargetActor)
{
	if (bIsDead || !IsValid(TargetActor) || TargetActor == this)
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const float Now = World->GetTimeSeconds();
	if (Now - LastFireTime < FireInterval)
	{
		return false;
	}

	const FVector TraceStart = IsValid(MuzzlePoint) ? MuzzlePoint->GetComponentLocation(): GetActorLocation() + FVector(0.0f, 0.0f, BaseEyeHeight);

	const FVector TargetPoint =TargetActor->GetActorLocation() + FVector(0.0f, 0.0f, TargetAimHeightOffset);
	const FVector ShotDirection = (TargetPoint - TraceStart).GetSafeNormal();
	const float TargetDistance = FVector::Distance(TraceStart, TargetPoint);

	if (ShotDirection.IsNearlyZero() || TargetDistance > WeaponRange)
	{
		return false;
	}

	LastFireTime = Now;

	if (FireMontage)
	{
		const float MontageDuration = PlayAnimMontage(FireMontage, 1.0f);
		if (MontageDuration <= 0.0f)
		{
			UE_LOG(LogTemp,Warning,TEXT("EnemySoldier: FireMontage non riprodotto. Controlla Skeleton e Slot."));
		}
	}

	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, TraceStart);
	}

	if (MuzzleFlashEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(World,MuzzleFlashEffect,TraceStart,ShotDirection.Rotation());
	}

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(EnemyRifleShot), true, this);
	QueryParams.AddIgnoredActor(this);

	FHitResult HitResult;
	// Il trace termina sul bersaglio: un muro prima del player blocca il colpo,
	// mentre un player che non blocca Visibility viene comunque danneggiato.
	const FVector TraceEnd = TargetPoint;
	const bool bBlockingHit = World->LineTraceSingleByChannel(HitResult,TraceStart,TraceEnd,ECC_Visibility,QueryParams);

	const bool bReachedTarget = !bBlockingHit || HitResult.GetActor() == TargetActor;

	if (bReachedTarget)
	{
		if (EnemyBulletClass)
		{
			if (UWorld* CurrentWorld = GetWorld())
			{
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.Owner = this;
				SpawnParameters.Instigator = this;
				SpawnParameters.SpawnCollisionHandlingOverride =ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				AEnemyBullet* SpawnedBullet =CurrentWorld->SpawnActor<AEnemyBullet>(EnemyBulletClass,TraceStart,ShotDirection.Rotation(),SpawnParameters);

				if (SpawnedBullet)
				{
					SpawnedBullet->SetDamage(AttackDamage);
				}
			}
		}
		else
		{
			//  soltanto se non assegno BP_EnemyBullet.
			UGameplayStatics::ApplyPointDamage(TargetActor,AttackDamage,ShotDirection,HitResult,GetController(),this,UDamageType::StaticClass());
		}
	
		//UGameplayStatics::ApplyPointDamage(TargetActor,AttackDamage,ShotDirection,HitResult,GetController(),this,UDamageType::StaticClass());
	}

	if (bDrawDebugShot)
	{
		const FVector DebugEnd = bBlockingHit ? HitResult.ImpactPoint : TargetPoint;
		DrawDebugLine(World,TraceStart,DebugEnd,bReachedTarget ? FColor::Green : FColor::Red,false,1.0f,0,1.5f);
	}

	return true;
}*/

bool AEnemySoldier::FireAtTarget(AActor* TargetActor)
{
	if (!TargetActor || bIsDead || bIsReacting || !MuzzlePoint)
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const float CurrentTime = World->GetTimeSeconds();

	if (CurrentTime - LastFireTime < FireInterval)  // tempo minimo dai 2 spari
	{
		return false;
	}

	const FVector MuzzleLocation = MuzzlePoint->GetComponentLocation();

	const FVector TargetPoint =TargetActor->GetActorLocation() + FVector(0.0f, 0.0f, TargetAimHeightOffset); //punti ideale allo sparo

	const FVector IdealDirection =(TargetPoint - MuzzleLocation).GetSafeNormal();

	const float TargetDistance =FVector::Distance(MuzzleLocation, TargetPoint);

	if (TargetDistance > WeaponRange)
	{
		return false;
	}

	// Controllo della visuale prima di sparare.
	FHitResult SightHit;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	const bool bSightBlocked = World->LineTraceSingleByChannel(SightHit,MuzzleLocation,TargetPoint,ECC_Visibility,QueryParams);

	if (bSightBlocked && SightHit.GetActor() != TargetActor)
	{
		return false;
	}

	if (!EnemyBulletClass)
	{
		UE_LOG(LogTemp,Warning,TEXT("EnemyBulletClass non assegnata in %s"),*GetName());

		return false;
	}

	// Se il player corre, il nemico diventa meno preciso.
	const float TargetSpeed = TargetActor->GetVelocity().Size2D();

	const float MovementAlpha = FMath::Clamp(TargetSpeed / 450.0f,0.0f,1.0f);

	const float CurrentSpreadDegrees =BaseSpreadDegrees +MovingTargetSpreadBonusDegrees * MovementAlpha;

	const FVector ShotDirection = FMath::VRandCone(IdealDirection,FMath::DegreesToRadians(CurrentSpreadDegrees)).GetSafeNormal();

	const FRotator SpawnRotation = ShotDirection.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride =ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AEnemyBullet* SpawnedBullet = World->SpawnActor<AEnemyBullet>(EnemyBulletClass,MuzzleLocation,SpawnRotation,SpawnParams);

	if (!SpawnedBullet)
	{
		return false;
	}

	SpawnedBullet->SetDamage(AttackDamage);
	LastFireTime = CurrentTime;

	if (FireMontage)
	{
		PlayAnimMontage(FireMontage);
	}

	if (bFireSoundEnable && FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this,FireSound,MuzzleLocation);
	}

	if (MuzzleFlashEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, MuzzleFlashEffect,MuzzleLocation,SpawnRotation);
	}
	//DrawDebugLine(World, MuzzleLocation, MuzzleLocation + ShotDirection * 800, FColor::Red, false, 0.2f, 0, 0.6f);
#if ENABLE_DRAW_DEBUG
	if (bDrawDebugShot)
	{
		//const FVector TracerEnd = MuzzleLocation + IdealDirection * TargetDistance;
		const FVector TracerEnd = MuzzleLocation + ShotDirection * TargetDistance;
		DrawDebugLine(World, MuzzleLocation,  TracerEnd, FColor::Red, false, 1.0f, 0, 0.90f);

	}
#endif

	return true;
}
