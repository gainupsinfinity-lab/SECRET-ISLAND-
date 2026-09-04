// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySoldierAnimInstance.h"
#include "EnemySoldier.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"

void UEnemySoldierAnimInstance::NativeInitializeAnimation()  //Verifica quale AEnemy sta usando l'Anim BP
{
	Super::NativeInitializeAnimation();  
	SoldierOwner = Cast<AEnemySoldier>(TryGetPawnOwner());  // Restituisce il Pawn proprietariofi AnimIstance
}

void UEnemySoldierAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	AEnemySoldier* Soldier = SoldierOwner.Get(); // dammi il punatore se è valido
	if (!Soldier)
	{
		Soldier = Cast<AEnemySoldier>(TryGetPawnOwner());  //recupero di sicurezza
		SoldierOwner = Soldier;
	}

	if (!Soldier)
	{
		Speed = 0.0f;
		Direction = 0.0f;
		bShouldMove = false;
		bIsInAir = false;
		bIsAiming = false;
		AimYaw = 0.0f;
		AimPitch = 0.0f;
		return;
	}

	const FVector Velocity = Soldier->GetVelocity(); 
	float TargetSpeed= Velocity.Size2D();
	Speed = TargetSpeed;
	//Speed = FMath::FInterpTo(Speed,TargetSpeed,DeltaSeconds,8.0f);
	bShouldMove = Speed > 3.0f;

	if (bShouldMove)
	{
		const FVector LocalVelocity =Soldier->GetActorTransform().InverseTransformVectorNoScale(Velocity); // velocità inziale in world  passaggio in locale
		Direction = FMath::RadiansToDegrees(FMath::Atan2(LocalVelocity.Y, LocalVelocity.X));  // convesrionevettore locale in angolo
	}
	else
	{
		Direction = 0.0f;
	}

	if (const UCharacterMovementComponent* Movement = Soldier->GetCharacterMovement())
	{
		bIsInAir = Movement->IsFalling();
	}

	bIsAiming = Soldier->IsAiming();
	//UE_LOG(LogTemp, Warning, TEXT("ANIM AIMING= %s | SOLDIER= %s"),bIsAiming ? TEXT("TRUE") : TEXT("FALSE"), Soldier->IsAiming()?TEXT("TRUE"):TEXT("FALSE"));

	if (bIsAiming && Soldier->GetController())  // pawn controllato da AiController 
	{	// calcolo differenza trra dove punta il controller e dove orientato fisicamente.evita cosi il BP di ruotare busto senza ruoatre il corpo
		const FRotator AimDelta =(Soldier->GetController()->GetControlRotation() - Soldier->GetActorRotation()).GetNormalized(); //(es. yaw Controller30- yaw Soldier 0=30
		AimYaw = FMath::Clamp(AimDelta.Yaw, -90.0f, 90.0f);			//hori															
		AimPitch = FMath::Clamp(AimDelta.Pitch, -60.0f, 60.0f);  // verti
	}
	else
	{
		AimYaw = 0.0f;
		AimPitch = 0.0f;
	}
}
