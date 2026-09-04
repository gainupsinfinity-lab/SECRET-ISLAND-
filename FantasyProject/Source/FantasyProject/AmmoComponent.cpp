// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoComponent.h"


UAmmoComponent::UAmmoComponent():
	MaxAmmo(50),
	InitialAmmo(50),
	CurrentAmmo(50)
{
	
	PrimaryComponentTick.bCanEverTick = false;

}


// Called  game starts
void UAmmoComponent::BeginPlay()
{
	Super::BeginPlay();
	MaxAmmo = FMath::Max(MaxAmmo, 1);

	CurrentAmmo = FMath::Clamp(InitialAmmo,0,MaxAmmo);// controllo del BP sempre max 0/50

	BroadcastAmmoChanged();
	
}

bool UAmmoComponent::CanFire() const
{
	return CurrentAmmo > 0;
}
bool UAmmoComponent::ConsumeRound()
{
	if (!CanFire())
	{
		return false;

	}
	--CurrentAmmo;

	BroadcastAmmoChanged();
	return true;
}

int32 UAmmoComponent::AddAmmo(int32 Amount)
{
	if (Amount <= 0 || IsFull())
	{
		return 0;
	}

	const int32 PreviousAmmo = CurrentAmmo;

	CurrentAmmo = FMath::Clamp(CurrentAmmo + Amount,0,MaxAmmo);

	const int32 AddedAmmo =CurrentAmmo - PreviousAmmo;

	if (AddedAmmo > 0)
	{
		BroadcastAmmoChanged();
	}

	return AddedAmmo;
}

bool UAmmoComponent::IsFull() const
{
	return CurrentAmmo >= MaxAmmo;
}

int32 UAmmoComponent::GetCurrentAmmo() const
{
	return CurrentAmmo;
}

int32 UAmmoComponent::GetMaxAmmo() const
{
	return MaxAmmo;
}

void UAmmoComponent::BroadcastAmmoChanged()
{
	OnAmmoChanged.Broadcast(CurrentAmmo,MaxAmmo);
}
