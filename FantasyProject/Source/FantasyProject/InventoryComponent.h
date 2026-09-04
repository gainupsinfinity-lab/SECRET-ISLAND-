// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryTypes.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);		// Delegate per alter Classi

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))						
class FANTASYPROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	bool AddItem(const FInventoryItem& NewItem);
	bool RemoveItem(FName ItemID, int32 QuantityToRemove);
	bool HasItem(FName ItemID, int32 RequiredQuantity = 1) const;
	int32 GetItemQuantity(FName ItemID) const;
	const TArray<FInventoryItem>& GetItems() const;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;

protected:
	virtual void BeginPlay() override;

	int32 FindItemIndex(FName ItemID) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FInventoryItem> Items;
};

