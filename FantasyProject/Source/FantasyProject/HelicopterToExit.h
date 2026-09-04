// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HelicopterToExit.generated.h"

class USceneComponent;
class USphereComponent;
class APlayerControl;


UCLASS()
class FANTASYPROJECT_API AHelicopterToExit : public AActor
{
	GENERATED_BODY()
	
public:	

	AHelicopterToExit();


protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Extraction|Components")
		USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Extraction|Components")
		USphereComponent* InteractionSphere;


	UFUNCTION()
		void OnInteractionBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,
			bool bFromSweep,const FHitResult& SweepResult);

	UFUNCTION()
		void OnInteractionEndOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);
	UFUNCTION(BlueprintImplementableEvent, Category = "Extraction") //evento
		void StartExtractionCinematic(APlayerControl* Player); //  filamto
private:
	bool bIsStartExtraction;


};
