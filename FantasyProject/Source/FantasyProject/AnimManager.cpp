// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimManager.h"
#include "Kismet/GameplayStatics.h"
#include "WaterBodyActor.h"
#include "WaterBodyComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"


namespace
{
	enum class EFootstepSurfaceBranch
	{
		Default,
		Sand,
		Arid,
		Water
	};

	EFootstepSurfaceBranch ResolveSurfaceBranch(UPhysicalMaterial* HitPhysMat)
	{
		const EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysMat);

		if (SurfaceType == EPS_Sand)
		{
			return EFootstepSurfaceBranch::Sand;
		}

		if (SurfaceType == EPS_Arid)
		{
			return EFootstepSurfaceBranch::Arid;
		}
		if (SurfaceType == EPS_Water)
		{
			return EFootstepSurfaceBranch::Water;
		}

		if (HitPhysMat) // mezzo physical material
		{
			const FString PhysMatName = HitPhysMat->GetName();
			if (PhysMatName.Contains(TEXT("Sand"), ESearchCase::IgnoreCase))
			{
				return EFootstepSurfaceBranch::Sand;
			}

			if (PhysMatName.Contains(TEXT("Arid"), ESearchCase::IgnoreCase) || PhysMatName.Contains(TEXT("Arido"), ESearchCase::IgnoreCase))
			{
				return EFootstepSurfaceBranch::Arid;
			}
			if (PhysMatName.Contains(TEXT("Water"), ESearchCase::IgnoreCase))
			{
				return EFootstepSurfaceBranch::Water;
			}
		}

		return EFootstepSurfaceBranch::Default;
	}
	bool IsActorInsideWater(AActor* Actor)  // riconoscimento W
	{
		if (!Actor)
		{
			return false;
		}

		UWorld* World = Actor->GetWorld();

		if (!World)
		{
			return false;
		}

		FVector ActorLocation = Actor->GetActorLocation();

		float FeetZ = ActorLocation.Z;

		ACharacter* Character = Cast<ACharacter>(Actor);

		if (Character && Character->GetCapsuleComponent())
		{
			FeetZ =ActorLocation.Z -Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		}

		for (TActorIterator<AWaterBody> It(World); It; ++It)
		{
			AWaterBody* WaterBody = *It;

			if (!WaterBody)
			{
				continue;
			}

			UWaterBodyComponent* WaterComponent =WaterBody->GetWaterBodyComponent();

			if (!WaterComponent)
			{
				continue;
			}

			FVector WaterSurfaceLocation;
			FVector WaterSurfaceNormal;
			FVector WaterVelocity;
			float WaterDepth = 0.0f;

			WaterComponent->GetWaterSurfaceInfoAtLocation(ActorLocation,WaterSurfaceLocation,WaterSurfaceNormal,WaterVelocity,WaterDepth,true);

			/*if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1,2.0f,FColor::Cyan,FString::Printf(TEXT("FEET Z %.1f | WATER Z %.1f"),FeetZ,WaterSurfaceLocation.Z)
				);
			}*/

			if (FeetZ <= WaterSurfaceLocation.Z)
			{
				return true;
			}
		}

		return false;
	}

}




UAnimManager::UAnimManager()
{
	

	ConstructorHelpers::FObjectFinder<USoundWave>Audio0Mov(TEXT("SoundWave'/Game/PROJECT/Audio/FootsetpCeramique.FootsetpCeramique'"));
	WalkSound = Audio0Mov.Object;
	ConstructorHelpers::FObjectFinder<USoundWave>Audio1Mov(TEXT("SoundWave'/Game/PROJECT/Audio/TestMove/Walk2.Walk2'"));
	RunSlowSound = Audio1Mov.Object;
	ConstructorHelpers::FObjectFinder<USoundWave>Audio2Mov(TEXT("SoundWave'/Game/PROJECT/Audio/TestMove/FastRun5.FastRun5'"));
	RunFastSound = Audio2Mov.Object;
	// jump Sound
	ConstructorHelpers::FObjectFinder<USoundWave>Audio0Jump(TEXT("SoundWave'/Game/PROJECT/Audio/Jump/Jump1.Jump1'"));
	JumpStartSound = Audio0Jump.Object;
	ConstructorHelpers::FObjectFinder<USoundWave>Audio1Jump(TEXT("SoundWave'/Game/PROJECT/Audio/Jump/JumpForward2.JumpForward2'"));
	JumpForwardSound = Audio1Jump.Object;
	ConstructorHelpers::FObjectFinder<USoundWave>Audio2Jump(TEXT("SoundWave'/Game/PROJECT/Audio/Jump/Land1.Land1'"));
	JumpLandSound = Audio2Jump.Object;

	ConstructorHelpers::FObjectFinder<USoundWave>Audio0Crouch(TEXT("SoundWave'/Game/PROJECT/Audio/Crouch/WalkCrounch.WalkCrounch'"));
	WalkCrouchSound = Audio0Crouch.Object;

	ConstructorHelpers::FObjectFinder<USoundWave>Audio1Crouch(TEXT("SoundWave'/Game/PROJECT/Audio/Crouch/SlowRunCrouch.SlowRunCrouch'"));
	SlowRunCrouchSound = Audio1Crouch.Object;

	MovementSound = { WalkSound,RunSlowSound,RunFastSound,JumpStartSound,JumpForwardSound,JumpLandSound,WalkCrouchSound,SlowRunCrouchSound };

	//MovementSound_Sand = MovementSound;
	ConstructorHelpers::FObjectFinder<USoundWave> Arid0(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/AudioPhysic/Arid/WalkArid.WalkArid'"));
	ConstructorHelpers::FObjectFinder<USoundWave> Arid1(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/AudioPhysic/Arid/RunSlowArid.RunSlowArid'"));
	ConstructorHelpers::FObjectFinder<USoundWave> Arid2(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/AudioPhysic/Arid/RunFastArid.RunFastArid'"));
	ConstructorHelpers::FObjectFinder<USoundWave> Arid3(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/Jump/Jump1.Jump1'"));
	ConstructorHelpers::FObjectFinder<USoundWave> Arid4(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/Jump/JumpForward2.JumpForward2'"));
	ConstructorHelpers::FObjectFinder<USoundWave> Arid5(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/Jump/Land1.Land1'"));
	ConstructorHelpers::FObjectFinder<USoundWave> Arid6(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/Crouch/WalkCrounch.WalkCrounch'"));
	ConstructorHelpers::FObjectFinder<USoundWave> Arid7(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/Crouch/SlowRunCrouch.SlowRunCrouch'"));

	/*if (!Arid0.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimManager: WalkArid non caricato. Controlla il path /Game/PROJECT/Audio/AudioPhysic/Arid/WalkArid."));
	}*/


	// Popoliamo l'array dell'Arido mantenendo lo stesso ordine (0-7)!
	MovementSound_Arid.Empty();

	// Assegnazione sicura elemento per elemento controllando il successo del caricamento
	MovementSound_Arid.Add(Arid0.Succeeded() ? Arid0.Object : WalkSound);
	MovementSound_Arid.Add(Arid1.Succeeded() ? Arid1.Object : RunSlowSound);
	MovementSound_Arid.Add(Arid2.Succeeded() ? Arid2.Object : RunFastSound);
	MovementSound_Arid.Add(Arid3.Succeeded() ? Arid3.Object : JumpStartSound);
	MovementSound_Arid.Add(Arid4.Succeeded() ? Arid4.Object : JumpForwardSound);
	MovementSound_Arid.Add(Arid5.Succeeded() ? Arid5.Object : JumpLandSound);
	MovementSound_Arid.Add(Arid6.Succeeded() ? Arid6.Object : WalkCrouchSound);
	MovementSound_Arid.Add(Arid7.Succeeded() ? Arid7.Object : SlowRunCrouchSound);

	ConstructorHelpers::FObjectFinder<USoundWave> Sand0(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/AudioPhysic/Sand/WalkSand.WalkSand'"));
	UE_LOG(LogTemp, Warning, TEXT("CARICAMENTO WalkSand: %s | Oggetto: %s"), Sand0.Succeeded() ? TEXT("RIUSCITO") : TEXT("FALLITO"), Sand0.Succeeded() ? *Sand0.Object->GetName() : TEXT("NULL"));
	ConstructorHelpers::FObjectFinder<USoundWave> Sand1(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/AudioPhysic/Sand/RunSlowSand.RunSlowSand'"));
	ConstructorHelpers::FObjectFinder<USoundWave> Sand2(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/AudioPhysic/Sand/RunFastSand.RunFastSand'"));
	ConstructorHelpers::FObjectFinder<USoundWave> Sand3(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/Jump/Jump1.Jump1'"));
	ConstructorHelpers::FObjectFinder<USoundWave> Sand4(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/Jump/JumpForward2.JumpForward2'"));
	ConstructorHelpers::FObjectFinder<USoundWave> Sand5(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/AudioPhysic/Sand/LandingSand.LandingSand'"));
	ConstructorHelpers::FObjectFinder<USoundWave> Sand6(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/AudioPhysic/Sand/CrouchWalkSlow.CrouchWalkSlow'"));
	ConstructorHelpers::FObjectFinder<USoundWave> Sand7(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/AudioPhysic/Sand/CouchRunSlow.CouchRunSlow'"));

	/*if (!Sand0.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimManager: WalkSens non caricato. Controlla il path /Game/PROJECT/Audio/AudioPhysic/Sand/WalkSand."));
	}*/

	MovementSound_Sand.Empty();

	MovementSound_Sand.Add(Sand0.Succeeded() ? Sand0.Object : WalkSound);
	MovementSound_Sand.Add(Sand1.Succeeded() ? Sand1.Object : RunSlowSound);
	MovementSound_Sand.Add(Sand2.Succeeded() ? Sand2.Object : RunFastSound);
	MovementSound_Sand.Add(Sand3.Succeeded() ? Sand3.Object : JumpStartSound);
	MovementSound_Sand.Add(Sand4.Succeeded() ? Sand4.Object : JumpForwardSound);
	MovementSound_Sand.Add(Sand5.Succeeded() ? Sand5.Object : JumpLandSound);
	MovementSound_Sand.Add(Sand6.Succeeded() ? Sand6.Object : WalkCrouchSound);
	MovementSound_Sand.Add(Sand7.Succeeded() ? Sand7.Object : SlowRunCrouchSound);



	ConstructorHelpers::FObjectFinder<USoundWave> Water0(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/AudioPhysic/Water/WaterWalk.WaterWalk'"));
	UE_LOG(LogTemp, Warning, TEXT("CARICAMENTO WalkSand: %s | Oggetto: %s"), Sand0.Succeeded() ? TEXT("RIUSCITO") : TEXT("FALLITO"), Sand0.Succeeded() ? *Sand0.Object->GetName() : TEXT("NULL"));
	ConstructorHelpers::FObjectFinder<USoundWave> Water1(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/AudioPhysic/Water/WaterRunSlow.WaterRunSlow'"));
	ConstructorHelpers::FObjectFinder<USoundWave> Water2(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/AudioPhysic/Water/WaterRunFast.WaterRunFast'"));
	ConstructorHelpers::FObjectFinder<USoundWave> Water3(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/Jump/Jump1.Jump1'"));
	ConstructorHelpers::FObjectFinder<USoundWave> Water4(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/Jump/JumpForward2.JumpForward2'"));
	ConstructorHelpers::FObjectFinder<USoundWave> Water5(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/AudioPhysic/Water/LandWater.LandWater'"));
	ConstructorHelpers::FObjectFinder<USoundWave> Water6(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/AudioPhysic/Water/WlaterCrouch.WlaterCrouch'"));
	ConstructorHelpers::FObjectFinder<USoundWave> Water7(TEXT("/Script/Engine.SoundWave'/Game/PROJECT/Audio/AudioPhysic/Water/WlaterCrouch.WlaterCrouch'"));

	MovementSound_Water.Empty();

	MovementSound_Water.Add(Water0.Succeeded() ? Water0.Object : WalkSound);
	MovementSound_Water.Add(Water1.Succeeded() ? Water1.Object : RunSlowSound);
	MovementSound_Water.Add(Water2.Succeeded() ? Water2.Object : RunFastSound);
	MovementSound_Water.Add(Water3.Succeeded() ? Water3.Object : JumpStartSound);
	MovementSound_Water.Add(Water4.Succeeded() ? Water4.Object : JumpForwardSound);
	MovementSound_Water.Add(Water5.Succeeded() ? Water5.Object : JumpLandSound);
	MovementSound_Water.Add(Water6.Succeeded() ? Water6.Object : WalkCrouchSound);
	MovementSound_Water.Add(Water7.Succeeded() ? Water7.Object : SlowRunCrouchSound);





}



void UAnimManager::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;
	
	AActor* Actor = MeshComp->GetOwner(); //  prendo il personaggio e torovo la laposizione
	if (!Actor) return;
	if (!MeshComp->GetWorld()) return;
	const bool bInWater = IsActorInsideWater(Actor);

	// Facciamo partire un raggio invisibile dai piedi del giocatore verso il basso
	FVector Start = Actor->GetActorLocation();
	FVector End = Start - FVector(0.0f, 0.0f, 150.0f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Actor);
	QueryParams.bReturnPhysicalMaterial = true; // Questo dice ad Unreal di leggere il Physical Material!

	FHitResult HitResult;
	UPhysicalMaterial* HitPhysMat = nullptr;

	// Eseguiamo il tracciamento
	if (MeshComp->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams))
	{
		if (HitResult.PhysMaterial.IsValid())
		{
			HitPhysMat = HitResult.PhysMaterial.Get();
			/*if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Superficie Esposta: %s"), *HitPhysMat->GetName()));
			}*/
		}
	}

	// Se l'indice del passo e valido (tra 0 e 7)
	if (Audio > -1 && Audio < 8)
	{
		USoundWave* SoundToPlay = nullptr;
		FString BranchName = TEXT("Default");
		EFootstepSurfaceBranch SurfaceBranch;
		if (bInWater)
		{
			SurfaceBranch = EFootstepSurfaceBranch::Water;
		}
		else
		{
			SurfaceBranch = ResolveSurfaceBranch(HitPhysMat);
		}


		// SAND
		if (SurfaceBranch == EFootstepSurfaceBranch::Sand)
		{
			BranchName = TEXT("Sand");

			if (MovementSound_Sand.IsValidIndex(Audio))
			{
				SoundToPlay = MovementSound_Sand[Audio];
			}
		}

		// ARID
		else if (SurfaceBranch == EFootstepSurfaceBranch::Arid)
		{
			BranchName = TEXT("Arid");

			if (MovementSound_Arid.IsValidIndex(Audio))
			{
				SoundToPlay = MovementSound_Arid[Audio];
			}
		}

		// WATER
		else if (SurfaceBranch == EFootstepSurfaceBranch::Water)
		{
			BranchName = TEXT("Water");

			if (MovementSound_Water.IsValidIndex(Audio))
			{
				SoundToPlay = MovementSound_Water[Audio];
			}
		}

		// DEFAULT
		else
		{
			BranchName = TEXT("Default");

			if (MovementSound.IsValidIndex(Audio))
			{
				SoundToPlay = MovementSound[Audio];
			}
		}
		
		
		
		// Riproduzione il suono scelto alla posizione del giocatore
		if (SoundToPlay != nullptr)
		{
			FVector Location = MeshComp->GetComponentLocation();
			/*if (GEngine)
			{
				const FString SurfaceName = HitPhysMat ? HitPhysMat->GetName() : TEXT("None");
				const EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysMat);
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, FString::Printf(
					TEXT("Audio %d | PhysMat %s | SurfaceType %d | Branch %s | Sound %s"),
					Audio,
					*SurfaceName,
					static_cast<int32>(SurfaceType),
					*BranchName,
					*SoundToPlay->GetName()));
			}*/
			PlayAudio(MeshComp, Location, SoundToPlay);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AnimManager: nessun suono valido per Audio %d."), Audio);
		}
	}
}

void UAnimManager::PlayAudio(const UObject* Object, FVector Location, USoundWave* SoundToPlay)
{
	if (!Object || !SoundToPlay)return;
	UGameplayStatics::PlaySoundAtLocation(Object, SoundToPlay, Location);
	
}
