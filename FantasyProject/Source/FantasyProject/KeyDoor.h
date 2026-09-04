// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryTypes.h"
#include "KeyDoor.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class USphereComponent;
class USoundBase;
class APlayerControl;

UCLASS()
class FANTASYPROJECT_API AKeyDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AKeyDoor();
	
	bool TryUseKey(FName KeyID); // controllo Id ricevuto dal player

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Components")
		USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Components")
		USceneComponent* DoorPivot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Components")
		UStaticMeshComponent* DoorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Components")
		USphereComponent* InteractionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Key")
		FName RequiredKeyID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Opening")
		FVector OpenOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Sound")
		USoundBase* UnlockSound;

private:
	UPROPERTY(VisibleInstanceOnly, Category = "Door")
		bool bIsOpen;

	UFUNCTION()  //player entra zona interazione
		void OnInteractionBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);

	UFUNCTION()// esec dalla zona
		void OnInteractionEndOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);
	FVector CloseDoorRelativePosition;

	void OpenDoor();
};
