// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryTypes.h"
#include "InventorySlotWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventorySlotClicked, FInventoryItem, ItemData);  // slot cliccato evidenzia i dati

UCLASS()
class FANTASYPROJECT_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetItemData(const FInventoryItem& NewItemData);

	UFUNCTION(BlueprintPure, Category = "Inventory")
		FInventoryItem GetItemData() const;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
		FOnInventorySlotClicked OnSlotClicked;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
		UButton* SlotButton;

	UPROPERTY(meta = (BindWidgetOptional))
		UImage* IconImage;

	UPROPERTY(meta = (BindWidgetOptional))
		UTextBlock* QuantityText;

	UPROPERTY(meta = (BindWidgetOptional))
		UTextBlock* NameText;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
		FInventoryItem ItemData;

	UFUNCTION()
		void HandleSlotClicked();  // chiamata allo slot
	
};
