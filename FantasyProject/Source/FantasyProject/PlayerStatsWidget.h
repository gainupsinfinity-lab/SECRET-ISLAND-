// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatsWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UBorder;
UENUM(BlueprintType)
enum class EPlayerNotificationType : uint8
{
	Info,
	Success,
	Warning,
	Error

};
UCLASS()
class FANTASYPROJECT_API UPlayerStatsWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Stats")
		void SetStats(float CurrentHealth, float InMaxHealth, float CurrentMana, float InMaxMana);

	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
		void SetAmmo(int32 CurrentAmmo, int32 MaxAmmo);

	UFUNCTION(BlueprintCallable, Category = "Stats")
		void ShowWarningMessage(const FText& Message);

	UFUNCTION(BlueprintCallable, Category = "UI/Notification")
		void ShowNotificationMessage(const FText& Message, EPlayerNotificationType Type, float Duration);

	UFUNCTION(BlueprintCallable, Category = "UI/Notification")
		void ShowInteractionPrompt(const FText& Message);

	UFUNCTION(BlueprintCallable, Category = "UI/Notification")
		void HideInteractionPrompt();

	UFUNCTION(BlueprintCallable, Category = "UI/Mission")
		void ShowMissionMessage(const FText& Message, float Duration = 5.0f);

protected:
	UPROPERTY(meta = (BindWidget))
		UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
		UProgressBar* ManaBar;

	UPROPERTY(meta = (BindWidgetOptional))
		UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidgetOptional))
		UTextBlock* ManaText;
	UPROPERTY(meta = (BindWidgetOptional))
		UTextBlock* WarningText;
	UPROPERTY(meta = (BindWidget))
		UBorder* NotificationPanel;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* NotificationText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* InteractionText;
	UPROPERTY(meta = (BindWidgetOptional))
		UBorder* MissionPanel;
	UPROPERTY(meta = (BindWidgetOptional))
		UTextBlock* MissionText;

	UPROPERTY(meta = (BindWidgetOptional))
		UBorder* AmmoPanel;
	UPROPERTY(meta = (BindWidgetOptional))
		UTextBlock* AmmoText;

private:
	FTimerHandle WarningMessageTimerHandle;
	void HideWarningMessage();
	FTimerHandle NotificationTimerHandle;
	void HideNotificationMessage();
	FTimerHandle MissionMessageTimerHandle;
	void HideMissionMessage();
};
