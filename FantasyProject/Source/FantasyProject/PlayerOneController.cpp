// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerOneController.h"

// Sets default values
APlayerOneController::APlayerOneController()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerOneController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerOneController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerOneController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

