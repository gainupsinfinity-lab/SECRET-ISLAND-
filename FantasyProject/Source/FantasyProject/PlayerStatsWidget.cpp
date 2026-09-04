// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatsWidget.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/TextBlock.h"

void UPlayerStatsWidget::SetStats(float CurrentHealth, float InMaxHealth, float CurrentMana, float InMaxMana)
{
	const float HealthPercent = InMaxHealth > 0.f ? CurrentHealth / InMaxHealth : 0.f;
	const float ManaPercent = InMaxMana > 0.f ? CurrentMana / InMaxMana : 0.f;

	if (HealthBar)
	{
		HealthBar->SetPercent(FMath::Clamp(HealthPercent, 0.f, 1.f));
	}

	if (ManaBar)
	{
		ManaBar->SetPercent(FMath::Clamp(ManaPercent, 0.f, 1.f));
	}

	if (HealthText)
	{
		HealthText->SetText(FText::FromString(FString::Printf(TEXT("Vita %.0f / %.0f"), CurrentHealth, InMaxHealth)));
	}

	if (ManaText)
	{
		//ManaText->SetText(FText::FromString(FString::Printf(TEXT("Mana %.0f / %.0f"), CurrentMana, InMaxMana)));
		const float ClampedManaPercent = FMath::Clamp(ManaPercent, 0.f, 1.f);
		const int32 ManaPercentInt = FMath::RoundToInt(ClampedManaPercent * 100.f);
		ManaText->SetText(FText::FromString(FString::Printf(TEXT("%d%%"), ManaPercentInt)));
	}
}
void UPlayerStatsWidget::ShowNotificationMessage(const FText& Message,EPlayerNotificationType Type,float Duration)
{
    if (!NotificationPanel || !NotificationText)
    {
        return;
    }

    FLinearColor MessageColor = FLinearColor::White;

    switch (Type)
    {
    case EPlayerNotificationType::Info:MessageColor = FLinearColor( 0.40f,0.85f,1.00f,1.00f);
        break;

    case EPlayerNotificationType::Success:MessageColor = FLinearColor(0.30f,1.00f,0.45f,1.00f);
        break;

    case EPlayerNotificationType::Warning:MessageColor = FLinearColor(1.00f,0.75f,0.15f,1.00f);
        break;

    case EPlayerNotificationType::Error:MessageColor = FLinearColor(1.00f,0.25f,0.20f,1.00f);
        break;
    }

    NotificationText->SetText(Message);

    NotificationText->SetColorAndOpacity(FSlateColor(MessageColor));

    NotificationPanel->SetVisibility(ESlateVisibility::HitTestInvisible);

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(NotificationTimerHandle);

        const float SafeDuration =FMath::Max(Duration, 0.1f);

        World->GetTimerManager().SetTimer(NotificationTimerHandle,this,&UPlayerStatsWidget::HideNotificationMessage,SafeDuration,false);
    }
}
void UPlayerStatsWidget::SetAmmo(int32 CurrentAmmo, int32 MaxAmmo)
{
    if (AmmoText)
    {
        AmmoText->SetText(FText::FromString(FString::Printf(TEXT("Munizioni: %d / %d"), CurrentAmmo, MaxAmmo)));
    }
    if (AmmoPanel)
    {
        AmmoPanel->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
}


void UPlayerStatsWidget::HideNotificationMessage()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(NotificationTimerHandle);
    }

    if (NotificationText)
    {
        NotificationText->SetText(FText::GetEmpty());
    }

    if (NotificationPanel)
    {
        NotificationPanel->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UPlayerStatsWidget::ShowInteractionPrompt(const FText& Message)
{
    if (!InteractionText)
    {
        return;
    }

    InteractionText->SetText(Message);

    InteractionText->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UPlayerStatsWidget::HideInteractionPrompt()
{
    if (!InteractionText)
    {
        return;
    }

    InteractionText->SetText(FText::GetEmpty());

    InteractionText->SetVisibility(ESlateVisibility::Collapsed);
}
void UPlayerStatsWidget::ShowMissionMessage(const FText& Message, float Duration)
{
    if (!MissionText)
    {
        return;
    }

    MissionText->SetText(Message);
    MissionText->SetColorAndOpacity(FSlateColor(FLinearColor(0.20f, 0.85f, 1.00f, 1.00f)));
    MissionText->SetVisibility(ESlateVisibility::HitTestInvisible);

    if (MissionPanel)
    {
        MissionPanel->SetVisibility(ESlateVisibility::HitTestInvisible);
    }

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(MissionMessageTimerHandle);
        World->GetTimerManager().SetTimer(MissionMessageTimerHandle,this,&UPlayerStatsWidget::HideMissionMessage,FMath::Max(Duration, 0.1f), false);
    }                                                                                 //Timer richiama HideMission
}

void UPlayerStatsWidget::HideMissionMessage()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(MissionMessageTimerHandle);
    }

    if (MissionText)
    {
        MissionText->SetText(FText::GetEmpty());
        MissionText->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (MissionPanel)
    {
        MissionPanel->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UPlayerStatsWidget::ShowWarningMessage(const FText& Message)
{
    if (!WarningText)
    {
        return;
    }
    WarningText->SetText(Message);WarningText->SetVisibility(ESlateVisibility::Visible);

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(WarningMessageTimerHandle);

        World->GetTimerManager().SetTimer(WarningMessageTimerHandle,this,&UPlayerStatsWidget::HideWarningMessage,2.0f,false);
    }
}

void UPlayerStatsWidget::HideWarningMessage()
{
    if (WarningText)
    {
        WarningText->SetVisibility(
            ESlateVisibility::Collapsed
        );
    }
}

