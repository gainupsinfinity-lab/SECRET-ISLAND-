// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"

UInventoryComponent::UInventoryComponent()   
{
	PrimaryComponentTick.bCanEverTick = false;  
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UInventoryComponent::AddItem(const FInventoryItem& NewItem)  //non aggiungeogbj invalidi/zero/quantità negativa
{
	if (!NewItem.IsValidItem() || NewItem.Quantity <= 0)
	{
		return false;
	}

	const int32 ExistingIndex = FindItemIndex(NewItem.ItemID);  // idetifica con ItemID
	if (ExistingIndex != INDEX_NONE) // verifica se esiste gia' 
	{
		FInventoryItem& ExistingItem = Items[ExistingIndex]; // prendol'array alla poszione ExitingIndex
		ExistingItem.Quantity += NewItem.Quantity; // aggiungo
		OnInventoryChanged.Broadcast();
		return true;
	}
	//non trova aggrea nuova voce nell'inventario
	Items.Add(NewItem);
	OnInventoryChanged.Broadcast();
	return true;
}

bool UInventoryComponent::RemoveItem(FName ItemID, int32 QuantityToRemove)
{
	if (ItemID.IsNone() || QuantityToRemove <= 0)
	{
		return false;
	}

	const int32 ExistingIndex = FindItemIndex(ItemID); // cerca oggetto
	if (ExistingIndex == INDEX_NONE)
	{
		return false;
	}

	FInventoryItem& ExistingItem = Items[ExistingIndex];
	if (ExistingItem.Quantity < QuantityToRemove)  // controllo quantità
	{
		return false;
	}

	ExistingItem.Quantity -= QuantityToRemove;
	if (ExistingItem.Quantity <= 0)
	{
		Items.RemoveAt(ExistingIndex);  // a zero tiro via la parte di array
	}

	OnInventoryChanged.Broadcast();
	return true;
}

bool UInventoryComponent::HasItem(FName ItemID, int32 RequiredQuantity) const
{
	return GetItemQuantity(ItemID) >= RequiredQuantity;// controllo quantità
}

int32 UInventoryComponent::GetItemQuantity(FName ItemID) const
{
	const int32 ExistingIndex = FindItemIndex(ItemID);  // ctrova indice
	if (ExistingIndex == INDEX_NONE)
	{
		return 0;
	}

	return Items[ExistingIndex].Quantity;  // elenca n oggetti posseduti
}

const TArray<FInventoryItem>& UInventoryComponent::GetItems() const
{
	return Items;  // da inventoryMainWidget  Inventory->Getitems->lista oggetti->crea slot
}

int32 UInventoryComponent::FindItemIndex(FName ItemID) const
{
	for (int32 Index = 0; Index < Items.Num(); ++Index)  // ricerca indice
	{
		if (Items[Index].ItemID == ItemID)
		{
			return Index;
		}
	}

	return INDEX_NONE;  // nessun indice valido
}

