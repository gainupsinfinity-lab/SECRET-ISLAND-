// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlotWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SlotButton)
	{
		SlotButton->OnClicked.RemoveDynamic(this, &UInventorySlotWidget::HandleSlotClicked);
		SlotButton->OnClicked.AddDynamic(this, &UInventorySlotWidget::HandleSlotClicked);
	}
}

void UInventorySlotWidget::SetItemData(const FInventoryItem& NewItemData)
{
	ItemData = NewItemData;

	if (NameText)
	{
		NameText->SetText(ItemData.ItemName);
	}

	if (QuantityText)
	{
		const bool bShowQuantity = ItemData.Quantity > 1;
		QuantityText->SetVisibility(bShowQuantity ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		QuantityText->SetText(FText::FromString(FString::Printf(TEXT("x%d"), ItemData.Quantity)));
	}

	if (IconImage)
	{
		if (ItemData.Icon)
		{
			IconImage->SetBrushFromTexture(ItemData.Icon);
			IconImage->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			IconImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

FInventoryItem UInventorySlotWidget::GetItemData() const
{
	return ItemData;
}

void UInventorySlotWidget::HandleSlotClicked()
{
	if (ItemData.IsValidItem())
	{
		OnSlotClicked.Broadcast(ItemData);
	}
}

