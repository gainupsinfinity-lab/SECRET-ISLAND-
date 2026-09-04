// Fill out your copyright notice in the Description page of Project Settings.


#include "KeyDoor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include  "PlayerControl.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

AKeyDoor::AKeyDoor() :
	Root(nullptr),
	DoorPivot(nullptr),
	DoorMesh(nullptr),
	InteractionSphere(nullptr),
	RequiredKeyID(TEXT("Key_Office")),
	OpenOffset(0.0f, -100.0f, 0.0f),
	UnlockSound(nullptr),
	bIsOpen(false),
	CloseDoorRelativePosition(FVector::ZeroVector)
{
 	
	PrimaryActorTick.bCanEverTick = false;

	//Root//
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	//Pivot Door//
	DoorPivot = CreateDefaultSubobject<USceneComponent>(TEXT("DoorPivot"));
	DoorPivot->SetupAttachment(Root);
	//Mesh//
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(DoorPivot);
	DoorMesh->SetMobility(EComponentMobility::Movable);

	//intercact//
	InteractionSphere =CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));

	InteractionSphere->SetupAttachment(Root);

	InteractionSphere->SetSphereRadius(180.0f);

	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);;
	InteractionSphere->SetGenerateOverlapEvents(true);

}


void AKeyDoor::BeginPlay()
{
	Super::BeginPlay();

	//posizione inizlale
	if (DoorPivot) 
	{
		CloseDoorRelativePosition = DoorPivot->GetRelativeLocation();
	}

	//entra
	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this,&AKeyDoor::OnInteractionBeginOverlap);

	//esce
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this,&AKeyDoor::OnInteractionEndOverlap);
	
}

bool AKeyDoor::TryUseKey(FName KeyID)
{
	if (bIsOpen) 
	{
		return false;
	}

	if (KeyID != RequiredKeyID) // confronto key required con id player
	{
		return false;
	}

	OpenDoor();
	return true;
}

void AKeyDoor::OpenDoor()
{
	if (bIsOpen || !DoorPivot)
	{
		return;
	}
	bIsOpen = true;

	//DoorPivot->SetRelativeRotation(FRotator(0.0f, OpenAngle, 0.0f));
	DoorPivot->SetRelativeLocation(CloseDoorRelativePosition + OpenOffset);
	if (UnlockSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, UnlockSound, GetActorLocation());
	}
}




void AKeyDoor::OnInteractionBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
	APlayerControl* Player = Cast< APlayerControl>(OtherActor); //veriifca che sia il player
	if (!Player)
	{
		return;
	}

	//solo capsula per evitare overlap altri obj
	if (OtherComp != Player->GetCapsuleComponent())
	{
		return;
	}

	Player->SetCurrentDoor(this);

	
}

void AKeyDoor::OnInteractionEndOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex)
{

	APlayerControl* Player = Cast< APlayerControl>(OtherActor); //veriifca che sia il player
	if (!Player)
	{
		return;
	}
	if (OtherComp != Player->GetCapsuleComponent())
	{
		return;
	}
	Player->ClearCurrentDoor(this);
}