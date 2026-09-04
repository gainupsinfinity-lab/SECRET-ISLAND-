// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSequenceBase.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Sound/SoundWave.h"
#include "AnimManager.generated.h"
#define EPS_Sand EPhysicalSurface::SurfaceType1
#define EPS_Arid EPhysicalSurface::SurfaceType2
#define EPS_Water EPhysicalSurface::SurfaceType3
/**
 *
 */
UCLASS()
class FANTASYPROJECT_API UAnimManager : public UAnimNotify
{
	GENERATED_BODY()
public:
	UAnimManager();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		bool GetSurface = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		int32 Audio = -1;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
private:
	USoundWave* WalkSound;
	USoundWave* RunSlowSound;
	USoundWave* RunFastSound;

	USoundWave* JumpStartSound;
	USoundWave* JumpForwardSound;
	USoundWave* JumpLandSound;

	USoundWave* WalkCrouchSound;
	USoundWave* SlowRunCrouchSound;

	UPROPERTY(Transient)  //trasient Array temporanei IMPEDISCE AD UNREAL  DI SALVARE NOTIFY VECCHIE COPIE
		TArray< USoundWave*>MovementSound; // sound Default
	UPROPERTY(Transient)
		TArray< USoundWave*>MovementSound_Sand;
	UPROPERTY(Transient)
		TArray< USoundWave*>MovementSound_Arid;
	UPROPERTY(Transient)
		TArray< USoundWave*>MovementSound_Water;

	UPROPERTY()
		UPhysicalMaterial* PM_Sand;
	UPROPERTY()
		UPhysicalMaterial* PM_Arid;
	UPROPERTY()
		UPhysicalMaterial* PM_Water;


	//void PlayAudio(const UObject* Object, FVector Location, int32 Ref); //oggetto-posizione.referenza
	void PlayAudio(const UObject* Object, FVector Location, USoundWave* SoundToPlay); //oggetto-posizione.refernza
};