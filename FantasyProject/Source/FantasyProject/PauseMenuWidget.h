// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

class UButton;
class USoundBase;

UCLASS()
class FANTASYPROJECT_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;  //di UserWidget runtime

	// Devono avere esattamente questi nomi nel WBP_PauseMenu
	UPROPERTY(meta = (BindWidget))
		UButton* ResumeButton;

	UPROPERTY(meta = (BindWidget))
		UButton* MainMenuButton;

	UPROPERTY(meta = (BindWidget))
		UButton* ExitButton;

	// Suono click assegnabile dal Blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pause Menu|Sound")
		USoundBase* ClickSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pause Menu|Sound")
		USoundBase* HoverSound;

private:

	UFUNCTION()
		void OnResumeClicked();

	UFUNCTION()
		void OnMainMenuClicked();

	UFUNCTION()
		void OnExitClicked();

	UFUNCTION()
		void OnResumeHovered();

	UFUNCTION()
		void OnMainMenuHovered();

	UFUNCTION()
		void OnExitHovered();


	void PlayClickSound();

	void PlayHoverSound();
	
};
