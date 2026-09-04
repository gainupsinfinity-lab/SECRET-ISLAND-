// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemPickup.h"
#include "PlayerControl.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"//#include "Engine/Engine.h"

AItemPickup::AItemPickup()
{
	PrimaryActorTick.bCanEverTick = false;
	WorldTransformBeforeHnadAttach = FTransform::Identity;
	bCollisionWasEnabledBeforeAttach = true;
	bIsAttacchedForPickup = false;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(RootComponent);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->SetSphereRadius(150.0f);
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ItemData.ItemID = TEXT("Key_Office");
	ItemData.ItemName = FText::FromString(TEXT("Chiave"));
	ItemData.ItemType = EInventoryItemType::KeyItem;
	ItemData.EquipmentSlot = EEquipmentSlot::None;
	ItemData.ConsumableEffect = EConsumableEffect::None;
	ItemData.Quantity = 1;
	ItemData.MaxStack = 99;
}

void AItemPickup::BeginPlay()
{
	Super::BeginPlay();

	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AItemPickup::OnInteractionBeginOverlap);  // entra in sphera
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &AItemPickup::OnInteractionEndOverlap); //esce
}
bool AItemPickup::AttachForPickup(USkeletalMeshComponent* CharacterMesh, FName SocketName)
{
	if (bIsAttacchedForPickup)
	{
		return true;
	}

	if (!CharacterMesh || !RootComponent || SocketName.IsNone() || !CharacterMesh->DoesSocketExist(SocketName))
	{
		return false;
	}

	WorldTransformBeforeHnadAttach = GetActorTransform();  // salavtaggio stato pos oroginale 
	bCollisionWasEnabledBeforeAttach = GetActorEnableCollision();// se collision attiva

	const bool bAttached = RootComponent->AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);  //attaccamento al Socket

	if (!bAttached)
	{
		return false;
	}

	SetActorEnableCollision(false);
	bIsAttacchedForPickup = true;
	return true;
}
void AItemPickup::RestoreAfterPickupAttach()
{
	if (!bIsAttacchedForPickup || !RootComponent)
	{
		return;
	}

	RootComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);  //raccolta non completata 

	SetActorTransform(WorldTransformBeforeHnadAttach, false, nullptr, ETeleportType::TeleportPhysics);  //stacca dalla mano pos originale

	SetActorEnableCollision(bCollisionWasEnabledBeforeAttach); //colission prima dei attach

	bIsAttacchedForPickup = false;
}

const FInventoryItem& AItemPickup::GetItemData() const  // altre classi che possono legegre i dati dell'oggetto 
{
	return ItemData;
}

FText AItemPickup::GetItemName() const
{
	return ItemData.ItemName;
}


bool AItemPickup::Pickup(APlayerControl* Player)
{
	if (!Player)
	{
		return false;
	}
	if (!ItemData.IsValidItem())
	{
		return false;
	}

	if (!Player->AddItemToInventory(ItemData))
	{
		return false;
	}

	USoundBase* SoundToPlay = ItemData.PickupSound ? ItemData.PickupSound : PickupSound;

	if (SoundToPlay)//(PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, GetActorLocation());
	}

	/*if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.5f, FColor::Green, FString::Printf(TEXT("Picked up: %s x%d"), *ItemData.ItemName.ToString(), ItemData.Quantity));
	}*/

	Destroy();
	return true;
}

void AItemPickup::OnInteractionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerControl* Player = Cast<APlayerControl>(OtherActor);
	if (!Player)
	{
		return;
	}

	Player->SetCurrentPickup(this);
}

void AItemPickup::OnInteractionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerControl* Player = Cast<APlayerControl>(OtherActor);
	if (!Player)
	{
		return;
	}

	Player->ClearCurrentPickup(this);// esce dalla sfea e cancella
}





