// Fill out your copyright notice in the Description page of Project Settings.


#include "HelicopterToExit.h"
#include "PlayerControl.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"


AHelicopterToExit::AHelicopterToExit():
	Root(nullptr),
	InteractionSphere(nullptr),
	bIsStartExtraction(false)
{
 	
	PrimaryActorTick.bCanEverTick = false;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);


	InteractionSphere =CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));

	InteractionSphere->SetupAttachment(Root);

	InteractionSphere->SetSphereRadius(300.0f);

	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);

	InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);

	InteractionSphere->SetGenerateOverlapEvents(true);

}

//spawned
void AHelicopterToExit::BeginPlay()
{
	Super::BeginPlay();
	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this,&AHelicopterToExit::OnInteractionBeginOverlap);

	InteractionSphere->OnComponentEndOverlap.AddDynamic(this,&AHelicopterToExit::OnInteractionEndOverlap);
	
}

// interazione con il player
void AHelicopterToExit::OnInteractionBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,
	UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
	APlayerControl* Player =Cast<APlayerControl>(OtherActor);

	if (!Player)
	{
		return;
	}

	if (OtherComp != Player->GetCapsuleComponent())
	{
		return;
	}
	if (!Player->HasMissionUSB())
	{
		Player->SetCurrentHelicopter(this);
		return;
	}
	if (bIsStartExtraction)
	{
		return;
	}
	bIsStartExtraction = true;

	StartExtractionCinematic(Player);

	
}


void AHelicopterToExit::OnInteractionEndOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex)
{
	APlayerControl* Player =Cast<APlayerControl>(OtherActor);

	if (!Player)
	{
		return;
	}

	if (OtherComp != Player->GetCapsuleComponent())
	{
		return;
	}
	

	Player->ClearCurrentHelicopter(this);
	
}
