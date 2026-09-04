// Fill out your copyright notice in the Description page of Project Settings.


#include "MineFieldProgressGate.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerControl.h"
#include "TimerManager.h"

// Sets default values
AMineFieldProgressGate::AMineFieldProgressGate()
{

	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));  // creazione tipo Uscenecomponent nel Bp component
	RootComponent = Root;  // prendo il root e poi i componenti fisici 

	MinefieldExitTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("MinefieldExitTrigger"));
	MinefieldExitTrigger->SetupAttachment(RootComponent);
	MinefieldExitTrigger->SetBoxExtent(FVector(250.0f, 250.0f, 150.0f));
	MinefieldExitTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MinefieldExitTrigger->SetGenerateOverlapEvents(true);
	MinefieldExitTrigger->SetCollisionResponseToAllChannels(ECR_Ignore); // ignora tutti obj
	MinefieldExitTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // se entra pawn genera evento overlap

	RequirementTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("RequirementTrigger"));
	RequirementTrigger->SetupAttachment(RootComponent);
	RequirementTrigger->SetBoxExtent(FVector(250.0f, 300.0f, 150.0f));
	RequirementTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RequirementTrigger->SetGenerateOverlapEvents(true);
	RequirementTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	RequirementTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	RoadBarrier = CreateDefaultSubobject<UBoxComponent>(TEXT("RoadBarrier"));
	RoadBarrier->SetupAttachment(RootComponent);
	RoadBarrier->SetBoxExtent(FVector(50.0f, 300.0f, 150.0f));  // grandezza box
	RoadBarrier->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	RoadBarrier->SetCollisionObjectType(ECC_WorldStatic);
	RoadBarrier->SetCollisionResponseToAllChannels(ECR_Ignore);
	RoadBarrier->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	RoadBarrier->SetHiddenInGame(true);  // non visibile in gioco

	bRequireMinefieldCompletion = true;
	MinefieldCompletedText = FText::FromString(TEXT("Sei riuscito ad attraversare il campo minato!"));
	MissionMessageDuration = 6.0f;
	RequirementCheckInterval = 0.25f;
	bMinefieldCompleted = false;
	bRoadUnlocked = false;

}


void AMineFieldProgressGate::BeginPlay()
{
	Super::BeginPlay();

	MinefieldExitTrigger->OnComponentBeginOverlap.AddDynamic(this,&AMineFieldProgressGate::OnMinefieldExitBeginOverlap);
	RequirementTrigger->OnComponentBeginOverlap.AddDynamic(this,&AMineFieldProgressGate::OnRequirementBeginOverlap);

	GetWorldTimerManager().SetTimer(RequirementCheckTimerHandle,this,&AMineFieldProgressGate::CheckUnlockRequirements,FMath::Max(RequirementCheckInterval, 0.05f),true);
	
}

void AMineFieldProgressGate::OnMinefieldExitBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,
	const FHitResult& SweepResult)
{
	APlayerControl* Player = Cast<APlayerControl>(OtherActor);
	if (!Player || Player->IsDead())
	{
		return;
	}

	if (!bMinefieldCompleted)
	{
		bMinefieldCompleted = true;
		Player->ShowMissionMessage(MinefieldCompletedText, MissionMessageDuration);
	}

	CheckUnlockRequirements();
}

void AMineFieldProgressGate::OnRequirementBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,
	bool bFromSweep,const FHitResult& SweepResult)
{
	APlayerControl* Player = Cast<APlayerControl>(OtherActor);
	if (!Player || Player->IsDead())
	{
		return;
	}

	CheckUnlockRequirements();
	if (!bRoadUnlocked)
	{
		ShowMissingRequirements(Player);
	}
}

void AMineFieldProgressGate::CheckUnlockRequirements()
{
	if (bRoadUnlocked)
	{
		return;
	}

	if (bRequireMinefieldCompletion && !bMinefieldCompleted)
	{
		return;
	}

	APlayerControl* Player = ResolvePlayer();
	if (!Player)
	{
		return;
	}

	if (Player->HasBackpackEquipped())
	{
		UnlockRoad();
	}
}

void AMineFieldProgressGate::UnlockRoad()
{
	if (bRoadUnlocked)
	{
		return;
	}

	bRoadUnlocked = true;
	RoadBarrier->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetWorldTimerManager().ClearTimer(RequirementCheckTimerHandle);

	if (APlayerControl* Player = ResolvePlayer())
	{
		Player->ShowProgressNotification(FText::FromString(TEXT("Strada sbloccata: puoi proseguire.")),3.0f);
	}
}

void AMineFieldProgressGate::ShowMissingRequirements(APlayerControl* Player) const
{
	if (!Player)
	{
		return;
	}

	if (bRequireMinefieldCompletion && !bMinefieldCompleted)
	{
		Player->ShowProgressNotification(FText::FromString(TEXT("Devi prima attraversare il campo minato.")),3.0f);
		return;
	}

	Player->ShowProgressNotification(FText::FromString(TEXT("Per proseguire devi raccogliere la borsa.")),3.0f);
}

APlayerControl* AMineFieldProgressGate::ResolvePlayer() const
{
	return Cast<APlayerControl>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

