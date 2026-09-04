// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControl.h"
void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ResumeButton)
	{
		ResumeButton->OnClicked.RemoveDynamic(this,&UPauseMenuWidget::OnResumeClicked);

		ResumeButton->OnClicked.AddDynamic(this,&UPauseMenuWidget::OnResumeClicked);
	}

	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.RemoveDynamic(this,&UPauseMenuWidget::OnMainMenuClicked);

		MainMenuButton->OnClicked.AddDynamic(this,&UPauseMenuWidget::OnMainMenuClicked);
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.RemoveDynamic(this,&UPauseMenuWidget::OnExitClicked);

		ExitButton->OnClicked.AddDynamic(this,&UPauseMenuWidget::OnExitClicked);
	}
	if (ResumeButton)
	{
		ResumeButton->OnHovered.RemoveDynamic(this, &UPauseMenuWidget::OnResumeHovered);
		ResumeButton->OnHovered.AddDynamic(this,&UPauseMenuWidget::OnResumeHovered);
	
	}

	if (MainMenuButton)
	{
		MainMenuButton->OnHovered.RemoveDynamic(this, &UPauseMenuWidget::OnMainMenuHovered);
		MainMenuButton->OnHovered.AddDynamic(this,&UPauseMenuWidget::OnMainMenuHovered);
	}

	if (ExitButton)
	{
		ExitButton->OnHovered.RemoveDynamic(this, &UPauseMenuWidget::OnExitHovered);
		ExitButton->OnHovered.AddDynamic(this,&UPauseMenuWidget::OnExitHovered);
	}
}


void UPauseMenuWidget::PlayClickSound()
{
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(this, ClickSound);
	}
}


void UPauseMenuWidget::OnResumeClicked()
{
	PlayClickSound();

	APlayerControl* Player =Cast<APlayerControl>(GetOwningPlayerPawn());

	if (Player)
	{
		Player->ClosePauseMenu();
	}
}


void UPauseMenuWidget::OnMainMenuClicked()
{
	PlayClickSound();

	//toglie la pausa prima di cambiare livello
	UGameplayStatics::SetGamePaused(this, false);

	UGameplayStatics::OpenLevel(this,FName(TEXT("L_MainMenu"))
	);
}


void UPauseMenuWidget::OnExitClicked()
{
	PlayClickSound();

	APlayerController* PlayerController = GetOwningPlayer();

	if (PlayerController)
	{															//Enum
		UKismetSystemLibrary::QuitGame(this,PlayerController,EQuitPreference::Quit,false);
	}
}
void UPauseMenuWidget::PlayHoverSound()
{
	if (HoverSound)
	{
		UGameplayStatics::PlaySound2D(this, HoverSound);
	}
}

void UPauseMenuWidget::OnResumeHovered()
{
	PlayHoverSound();
}

void UPauseMenuWidget::OnMainMenuHovered()
{
	PlayHoverSound();
}

void UPauseMenuWidget::OnExitHovered()
{
	PlayHoverSound();
}

