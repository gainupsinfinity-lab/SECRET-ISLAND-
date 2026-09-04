// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryTypes.generated.h"

class USoundBase;
class UTexture2D;

UENUM(BlueprintType)
enum class EInventoryItemType : uint8
{
	Consumable UMETA(DisplayName = "Consumable"),
	Equipment UMETA(DisplayName = "Equipment"),
	KeyItem UMETA(DisplayName = "Key Item"),
	Material UMETA(DisplayName = "Material")
};

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	None UMETA(DisplayName = "None"),
	Weapon UMETA(DisplayName = "Weapon"),
	Shield UMETA(DisplayName = "Shield"),
	Head UMETA(DisplayName = "Head"),
	Chest UMETA(DisplayName = "Chest"),
	Legs UMETA(DisplayName = "Legs"),
	Backpack UMETA(DisplayName = "Backpack"),
	Grenade UMETA(DisplayName="Grenade")

};

UENUM(BlueprintType)
enum class EConsumableEffect : uint8
{
	None UMETA(DisplayName = "None"),
	Health UMETA(DisplayName = "Health"),
	Mana UMETA(DisplayName = "Mana"),
	Ammo UMETA(DisplayName = "Ammo")
};

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		FName ItemID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		EInventoryItemType ItemType = EInventoryItemType::KeyItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		EEquipmentSlot EquipmentSlot = EEquipmentSlot::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		EConsumableEffect ConsumableEffect = EConsumableEffect::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ClampMin = "1"))
		int32 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ClampMin = "1"))
		int32 MaxStack = 99;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		float HealthAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		float ManaAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item",meta=(ClampMin="0",UImin="0"))
		int32 AmmoAmount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		UTexture2D* Icon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* UseSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* PickupSound = nullptr;

	bool IsValidItem() const
	{
		return !ItemID.IsNone();
	}
};