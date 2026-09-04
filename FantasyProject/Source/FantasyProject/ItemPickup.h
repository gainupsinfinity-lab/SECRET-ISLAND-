// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryTypes.h"
#include "ItemPickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class USoundBase;
class APlayerControl;
class USkeletalMeshComponent;

UCLASS()
class FANTASYPROJECT_API AItemPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	AItemPickup();

	const FInventoryItem& GetItemData() const;
	FText GetItemName() const;

	
	bool Pickup(APlayerControl* Player);
	bool AttachForPickup(USkeletalMeshComponent* CharacterMesh, FName SocketName);
	void RestoreAfterPickupAttach();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnInteractionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* InteractionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FInventoryItem ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* PickupSound;
private:
	FTransform WorldTransformBeforeHnadAttach;
	bool bCollisionWasEnabledBeforeAttach;
	bool bIsAttacchedForPickup;
};

