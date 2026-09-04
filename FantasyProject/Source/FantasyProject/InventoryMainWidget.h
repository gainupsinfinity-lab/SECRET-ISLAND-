// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryTypes.h"
#include "InventoryMainWidget.generated.h"

class UButton;
class UInventoryComponent;
class UInventorySlotWidget;
class UTextBlock;
class UWrapBox;

UENUM(BlueprintType)
enum class EInventoryFilter : uint8
{
	All UMETA(DisplayName = "All"),
	Consumables UMETA(DisplayName = "Consumables"),
	Equipment UMETA(DisplayName = "Equipment"),
	Keys UMETA(DisplayName = "Keys"),
	Materials UMETA(DisplayName = "Materials")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryItemRequested, FInventoryItem, ItemData);      //Delegate 3 eventi OnuseItemRequest,ONEquipp/OnClose
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryCloseRequested);

UCLASS()
class FANTASYPROJECT_API UInventoryMainWidget : public UUserWidget  //UIWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetInventoryComponent(UInventoryComponent* NewInventoryComponent);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void RefreshInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetFilter(EInventoryFilter NewFilter);

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
		FOnInventoryItemRequested OnUseItemRequested;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
		FOnInventoryItemRequested OnEquipItemRequested;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
		FOnInventoryCloseRequested OnCloseRequested;
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
		UWrapBox* InventoryGrid;  //In riferimento al widget blupirnt 

	UPROPERTY(meta = (BindWidgetOptional)) // funziona anche se non c'e nel Bp
		UTextBlock* SelectedItemNameText;

	UPROPERTY(meta = (BindWidgetOptional))
		UTextBlock* SelectedItemTypeText;

	UPROPERTY(meta = (BindWidgetOptional))
		UTextBlock* SelectedItemDescriptionText;

	UPROPERTY(meta = (BindWidgetOptional))
		UTextBlock* SelectedItemQuantityText;

	UPROPERTY(meta = (BindWidgetOptional))
		UButton* UseButton;

	UPROPERTY(meta = (BindWidgetOptional))
		UButton* EquipButton;

	UPROPERTY(meta = (BindWidgetOptional))
		UButton* FilterAllButton;

	UPROPERTY(meta = (BindWidgetOptional))
		UButton* FilterConsumablesButton;

	UPROPERTY(meta = (BindWidgetOptional))
		UButton* FilterEquipmentButton;

	UPROPERTY(meta = (BindWidgetOptional))
		UButton* FilterKeysButton;

	UPROPERTY(meta = (BindWidgetOptional))
		UButton* FilterMaterialsButton;

	UPROPERTY(meta = (BindWidgetOptional))
		UButton* CloseButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
		EInventoryFilter ActiveFilter;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
		FInventoryItem SelectedItem;

	UPROPERTY()
		UInventoryComponent* BoundInventoryComponent;

private:
	UFUNCTION()
		void HandleSlotClicked(FInventoryItem ItemData);

	UFUNCTION()
		void HandleUseClicked();

	UFUNCTION()
		void HandleEquipClicked();

	UFUNCTION()
		void ShowAllItems();

	UFUNCTION()
		void ShowConsumables();

	UFUNCTION()
		void ShowEquipment();

	UFUNCTION()
		void ShowKeys();

	UFUNCTION()
		void ShowMaterials();

	UFUNCTION()
		void HandleCloseClicked();

	bool DoesItemPassFilter(const FInventoryItem& Item) const;
	FText GetItemTypeText(const FInventoryItem& Item) const;
	void UpdateSelectedItemDetails();
	
};
