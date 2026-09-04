// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryMainWidget.h"
#include "InventoryComponent.h"
#include "InventorySlotWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"


void UInventoryMainWidget::NativeConstruct()
{
	Super::NativeConstruct(); // creazione widget

	ActiveFilter = EInventoryFilter::All;  // attivazione/mostra tutto

	//**COLL BUTTON  con Delegate**/   -prima Remove per evitare la registrazione 2 volte stesso evento

	if (UseButton)
	{
		UseButton->OnClicked.RemoveDynamic(this, &UInventoryMainWidget::HandleUseClicked);
		UseButton->OnClicked.AddDynamic(this, &UInventoryMainWidget::HandleUseClicked);
	}

	if (EquipButton)
	{
		EquipButton->OnClicked.RemoveDynamic(this, &UInventoryMainWidget::HandleEquipClicked);
		EquipButton->OnClicked.AddDynamic(this, &UInventoryMainWidget::HandleEquipClicked);
	}

	if (FilterAllButton)
	{
		FilterAllButton->OnClicked.RemoveDynamic(this, &UInventoryMainWidget::ShowAllItems);
		FilterAllButton->OnClicked.AddDynamic(this, &UInventoryMainWidget::ShowAllItems);
	}

	if (FilterConsumablesButton)
	{
		FilterConsumablesButton->OnClicked.RemoveDynamic(this, &UInventoryMainWidget::ShowConsumables);
		FilterConsumablesButton->OnClicked.AddDynamic(this, &UInventoryMainWidget::ShowConsumables);
	}

	if (FilterEquipmentButton)
	{
		FilterEquipmentButton->OnClicked.RemoveDynamic(this, &UInventoryMainWidget::ShowEquipment);
		FilterEquipmentButton->OnClicked.AddDynamic(this, &UInventoryMainWidget::ShowEquipment);
	}
	if (CloseButton)
	{
		CloseButton->OnClicked.RemoveDynamic(this, &UInventoryMainWidget::HandleCloseClicked);
		CloseButton->OnClicked.AddDynamic(this, &UInventoryMainWidget::HandleCloseClicked);
	}

	if (FilterKeysButton)
	{
		FilterKeysButton->OnClicked.RemoveDynamic(this, &UInventoryMainWidget::ShowKeys);
		FilterKeysButton->OnClicked.AddDynamic(this, &UInventoryMainWidget::ShowKeys);
	}

	if (FilterMaterialsButton)
	{
		FilterMaterialsButton->OnClicked.RemoveDynamic(this, &UInventoryMainWidget::ShowMaterials);
		FilterMaterialsButton->OnClicked.AddDynamic(this, &UInventoryMainWidget::ShowMaterials);
	}

	UpdateSelectedItemDetails();
}

void UInventoryMainWidget::SetInventoryComponent(UInventoryComponent* NewInventoryComponent)
{
	BoundInventoryComponent = NewInventoryComponent;  //widget riceve inventario e salava
	RefreshInventory();
}

void UInventoryMainWidget::RefreshInventory()
{
	if (!InventoryGrid)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryMainWidget: InventoryGrid non trovato. Nel Widget Blueprint crea un WrapBox chiamato esattamente InventoryGrid."));
		return;
	}

	InventoryGrid->ClearChildren();  // cancellazione vecchi slot

	if (!BoundInventoryComponent)
	{
		return;
	}

	if (!SlotWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryMainWidget: SlotWidgetClass non assegnata. Assegna WBP_InventorySlot nel Widget Blueprint."));
		return;
	}

	const TArray<FInventoryItem>& Items = BoundInventoryComponent->GetItems(); //recupero obj da inventory
	if (SelectedItem.IsValidItem())  // se esiste un oggetto nell'inventario
	{
		bool bSelectedItemStillExists = false;
		for (const FInventoryItem& Item : Items)  // scorrimento oggetti
		{
			if (Item.ItemID == SelectedItem.ItemID)
			{
				SelectedItem = Item;  // se esiste 
				bSelectedItemStillExists = true; //
				break;
			}
		}

		if (!bSelectedItemStillExists)
		{
			SelectedItem = FInventoryItem();  // se non esiste deselzioni
		}
	}

	for (const FInventoryItem& Item : Items)
	{
		if (!Item.IsValidItem() || !DoesItemPassFilter(Item)) //controllo se non passa il filtro
		{
			continue;
		}

		UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(this, SlotWidgetClass);  //creazione slot
		if (!SlotWidget)
		{
			continue;
		}

		SlotWidget->SetItemData(Item); //set item
		SlotWidget->OnSlotClicked.AddDynamic(this, &UInventoryMainWidget::HandleSlotClicked);  // coll all'ìevento click
		InventoryGrid->AddChildToWrapBox(SlotWidget); // aggiunge slot all wrapbox
	}

	UpdateSelectedItemDetails();
}

void UInventoryMainWidget::SetFilter(EInventoryFilter NewFilter)
{
	ActiveFilter = NewFilter; //ricostruzione filtro con nuova codnizione
	RefreshInventory();
}

void UInventoryMainWidget::HandleSlotClicked(FInventoryItem ItemData) //click slo->itemdata->sel->Upadate
{
	SelectedItem = ItemData;
	UpdateSelectedItemDetails();
}

void UInventoryMainWidget::HandleUseClicked()
{
	if (SelectedItem.IsValidItem() && (SelectedItem.ItemType == EInventoryItemType::Consumable || SelectedItem.ItemType == EInventoryItemType::KeyItem))
	{
		
			OnUseItemRequested.Broadcast(SelectedItem);
		
	}
}

void UInventoryMainWidget::HandleEquipClicked()
{
	if (SelectedItem.IsValidItem() && SelectedItem.ItemType == EInventoryItemType::Equipment)
	{
		OnEquipItemRequested.Broadcast(SelectedItem);  //se equipmente EQUIPAGGIA 
	}
}

void UInventoryMainWidget::HandleCloseClicked()
{
	
	OnCloseRequested.Broadcast();  // Chiude la richiesta
	
}

void UInventoryMainWidget::ShowAllItems()
{
	SetFilter(EInventoryFilter::All);
}

void UInventoryMainWidget::ShowConsumables()
{
	SetFilter(EInventoryFilter::Consumables);
}

void UInventoryMainWidget::ShowEquipment()
{
	SetFilter(EInventoryFilter::Equipment);
}

void UInventoryMainWidget::ShowKeys()
{
	SetFilter(EInventoryFilter::Keys);
}

void UInventoryMainWidget::ShowMaterials()
{
	SetFilter(EInventoryFilter::Materials);
}

bool UInventoryMainWidget::DoesItemPassFilter(const FInventoryItem& Item) const
{
	switch (ActiveFilter)
	{
	case EInventoryFilter::Consumables:
		return Item.ItemType == EInventoryItemType::Consumable;
	case EInventoryFilter::Equipment:
		return Item.ItemType == EInventoryItemType::Equipment;
	case EInventoryFilter::Keys:
		return Item.ItemType == EInventoryItemType::KeyItem;
	case EInventoryFilter::Materials:
		return Item.ItemType == EInventoryItemType::Material;
	case EInventoryFilter::All:
	default:
		return true;
	}
}

FText UInventoryMainWidget::GetItemTypeText(const FInventoryItem& Item) const  // riproduzione a testo
{
	switch (Item.ItemType)
	{
	case EInventoryItemType::Consumable:
		return FText::FromString(TEXT("Consumabile"));
	case EInventoryItemType::Equipment:
		return FText::FromString(TEXT("Equipaggiamento"));
	case EInventoryItemType::KeyItem:
		return FText::FromString(TEXT("Oggetto chiave"));
	case EInventoryItemType::Material:
		return FText::FromString(TEXT("Materiale"));
	default:
		return FText::FromString(TEXT("Oggetto"));
	}
}

void UInventoryMainWidget::UpdateSelectedItemDetails()
{
	const bool bHasSelectedItem = SelectedItem.IsValidItem();

	if (SelectedItemNameText)
	{
		SelectedItemNameText->SetText(bHasSelectedItem ? SelectedItem.ItemName : FText::FromString(TEXT("Nessun oggetto selezionato")));
	}
	/*
	if (SelectedItemDescriptionText)
	{
		SelectedItemDescriptionText->SetText(bHasSelectedItem ? (SelectedItem.ItemDescription.IsEmpty() ? FText::FromString(TEXT("Nessuna Descrizione")): SelectedItem.ItemDescription : FText::GetEmpty()));
	}*/
	if (SelectedItemDescriptionText)
	{
		if (!bHasSelectedItem)
		{
			SelectedItemDescriptionText->SetText(FText::GetEmpty());
		}
		else if (SelectedItem.ItemDescription.IsEmpty())
		{
			SelectedItemDescriptionText->SetText(FText::FromString(TEXT("Nessuna Descrizione")));
		}
		else
		{
			SelectedItemDescriptionText->SetText(SelectedItem.ItemDescription);
		}
	}





	if (SelectedItemNameText)
	{
		SelectedItemNameText->SetText(bHasSelectedItem ? SelectedItem.ItemName : FText::FromString(TEXT("Nessun oggetto selezionato")));
	}


	if (SelectedItemQuantityText)
	{
		SelectedItemQuantityText->SetText(bHasSelectedItem ? FText::FromString(FString::Printf(TEXT("Quantita': %d"), SelectedItem.Quantity)) : FText::GetEmpty());
	}

	if (UseButton)
	{
		UseButton->SetIsEnabled(bHasSelectedItem && (SelectedItem.ItemType == EInventoryItemType::Consumable || SelectedItem.ItemType == EInventoryItemType::KeyItem));
	}

	if (EquipButton)
	{
		EquipButton->SetIsEnabled(bHasSelectedItem && SelectedItem.ItemType == EInventoryItemType::Equipment);
	}
}

