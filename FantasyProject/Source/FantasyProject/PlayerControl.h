// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "InventoryTypes.h"
#include "ThrownGrenade.h"
#include "Sound/SoundBase.h"
#include "GameFramework/Character.h"
#include "PlayerControl.generated.h"


class USpringArmComponent;
class UCameraComponent;
class AItemPickup;
class UInventoryComponent;
class UAmmoComponent;
class UInventoryMainWidget;
class UPlayerStatsWidget;
class UAnimMontage;
class UStaticMeshComponent;
class USphereComponent;
class APistolWeapon;
class UUserWidget;
class UWidget;
class AThrownGrenade;
class UPauseMenuWidget;
class  AKeyDoor;
class AHelicopterToExit;
struct FHitResult;

UCLASS()
class FANTASYPROJECT_API APlayerControl : public ACharacter  //erdita CapuselCol,SkeletahlMesh, CharacterMovComp,Mov,Controller/Posses
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerControl();
	void ShowDebugWarning(const FString& Message);

	UFUNCTION(BlueprintPure, Category = "Progression")
		bool HasBackpackEquipped() const;  //stato backpack se raccolto

	UFUNCTION(BlueprintCallable, Category="Progression|UI")
		void ShowMissionMessage(const FText& message, float Duration = 5.0);

	UFUNCTION(BlueprintCallable, Category = "Progression|UI")
		void ShowProgressNotification(const FText& message, float Duration = 3.0);

	UFUNCTION(BlueprintPure, Category = "Progression|Mission")
		bool HasMissionUSB() const;  //stato recupero USB


	UFUNCTION(BlueprintCallable, Category = "Cinematic")
		void PrepareForExtracionCinematic();

	UFUNCTION(BlueprintCallable, Category = "UI")
		void HideGameplayHUD();

	UFUNCTION(BlueprintCallable, Category = "UI")
		void ShowGameplayHUD();


	

	

protected:
	
	virtual void BeginPlay() override;  // funzioni native

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;  //funzioni native
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override; //funzioni native
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)override; //funzioni native
	void FMoveToX(float value);
	void FMoveToY(float value);
	void FRotMouseX(float value);
	bool IsWallBlockingMovement(const FVector& Direction) const;
	void FLookUpDown(float value);
	void FScroll(float value);
	void FFire();
	//Action
	void FRun();
	void FNotRun();
	void FJump();
	void FNotJump();
	void FCrouch();
	void FNotCrouch();
	void FGun();
	void FInteract();
	void FUseItem();
	void FToggleInventory();
	UFUNCTION()
		void RefreshInventoryUI();
	UFUNCTION()
		void HandleUseItemFromUI(FInventoryItem ItemData);
	UFUNCTION()
		void HandleEquipItemFromUI(FInventoryItem ItemData);
	UFUNCTION()
		void HandleCloseInventoryFromUI();

	UFUNCTION(BlueprintCallable, Category = "Landmine")  //ACTION
		void ApplyLandmineExplosion(bool bLethalHit, float DemagePercent);

	UFUNCTION(BlueprintPure, Category = "Landmine")
		bool IsDead() const;
	UFUNCTION(BlueprintPure, Category = "Aim")
		bool IsAiming() const
	{
		return bIsAiming;
	}
	UFUNCTION(BlueprintCallable, Category = "Pause Menu")
		void OpenPauseMenu();

	UFUNCTION(BlueprintCallable, Category = "Pause Menu")
		void ClosePauseMenu();

	void UpdateIWR();
	//void StopFastRun();
	void StartFastRun();
	void ApplyMovementSpeed();
	//void DoRealJump();
	FTimerHandle FastRunTimerHandle;
	FTimerHandle JumpDelayTimerHandle;
	FTimerHandle LandmineStunTimerHandle;
	FTimerHandle RollTimerHandle;
	FTimerHandle GrenadeThrowTimerHandle;
	FTimerHandle ReturnToMainMenuTimerHandle;
	float _Speed;
	float _IWR;
	bool _isInAir;
	bool _isJumping;
	bool _wasInAir;
	bool _isForwardJump;
	float _VerticalVelocity;
	float _DeltaTime;
	float _AirControl;
	bool _GunLoadSoundPLayed;
	void PlayGunLoadSound();
	void PlayCrouchSound();
	bool AddItemToInventory(const FInventoryItem& ItemData);
	void SetCurrentPickup(AItemPickup* NewPickup);
	void ClearCurrentPickup(AItemPickup* PickupToClear);
	void SetCurrentDoor(AKeyDoor* NewDoor);
	void ClearCurrentDoor(AKeyDoor* DoorToClaer);
	void SetCurrentHelicopter(AHelicopterToExit* NewHelicopter);
	void ClearCurrentHelicopter(AHelicopterToExit* HelicopterToClear);
	UInventoryComponent* GetInventoryComponent() const;
	bool UseConsumableItem(FInventoryItem ItemData);
	void RestoreHealth(float Amount);
	void RestoreMana(float Amount);
	void RefreshPlayerStatsUI();
	UFUNCTION() // collegata addDynamic
		void RefreshAmmoUI(int32 CurrentAmmo, int32 MaxAmmo);  // ricezione n colpi
	void ApplyMovementManaDrain(float DeltaTime);
	bool HasUsableMana() const;
	void ForceWalkBecauseManaIsEmpty();
	void FToggleCrouch();
	void FStartAim();
	void FStopAim();
	void FTestAimTrace();// tiro di prova 
	void SetAimingRotationMode(bool bEnable);
	void FRoll();
	void EndRoll();
	void TogglePauseMenu();


private:
	float CapsuleRadius;
	float CapsuleHeight;
	float jumpVelocity;
	float MoveinputX;
	float MoveinputY;
	bool isGunVisible;
	bool isCrouching;
	bool isFastRunning;
	bool isRunPressed;

	UPROPERTY()
		AItemPickup* CurrentPickup;
	UPROPERTY()
		AItemPickup* PendingPickup;

	UPROPERTY()
		AKeyDoor* CurrentDoor;

	UPROPERTY()
		AHelicopterToExit* CurrentHelicopter;


	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Interaction|Pickup", meta = (AllowPrivateAccess = "true"))
		bool bIsPickingUp;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Interaction|Pickup", meta = (AllowPrivateAccess = "true"))
		bool bPickupObjectAttachedToHand;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Landmine| State", meta = (AllowPrivateAccess = "true"))
		bool  bIsDead;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Landmine| State", meta = (AllowPrivateAccess = "true"))
		bool bIsStunned;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Movment|Roll", meta = (AllowPrivateAccess = "true"))
		bool bIsRolling;

	bool bInventoryVisible;

	void CreateInventoryWidgetIfNeeded();

	void CreatePlayerStatsWidgetIfNeeded();
	
	UFUNCTION()
		void HandlePickupMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	void HandlePickupMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void RestoreMovementAfterPickup();
	void CompletePendingPickup();
	void EquipBackpack();
	void LockMovmentForLadnmine();
	void StartLandmineStun();
	void EndLandmineStun();
	void DieFromLandmine();
	void DieFromGunFire();
	void EnterDeathState(UAnimMontage* DeathMontageToPlay);
	void ReturnToMainMenuAfterDeath();
	void CreateAndAttachWeapon();
	void HideWeaponForPickup();
	void RestoreWeaponAfterPickup();
	void CreateAttachGrenadeHeld();
	void EquipGrenade();
	void FThrowGrenade();
	void EndGrenadeThrow();
	bool bRestoreWeaponAfterPickup; //se l'arma viene estratta prima della raccolta
	bool bRestoreWaponAfetrGranade;
	bool bGmaeplayHUDVisible;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		UInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
		UAmmoComponent* AmmoComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		TSubclassOf<UInventoryMainWidget> InventoryWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		TSubclassOf<UPlayerStatsWidget> PlayerStatsWidgetClass;

	UPROPERTY()
		UInventoryMainWidget* InventoryWidget;

	UPROPERTY() 
		UPlayerStatsWidget* PlayerStatsWidget;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pause Menu")
		TSubclassOf<UPauseMenuWidget> PauseMenuWidgetClass;

	UPROPERTY(Transient)
		UPauseMenuWidget* PauseMenuWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float MaxMana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float Mana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
		USpringArmComponent* PivotCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		UCameraComponent* CamPlayerOne;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotSpeedMouse")
		float mouseSpeedX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotSpeedMouse")
		float mouseSpeedY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|limits")
		float MinCameraPitch= -65.0f; //-39

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|limits")
		float MaxCameraPitch = 55.0f; //10

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Zoom",meta=(ClampMin="0.0")) //impedisce valori negativi
		float CameraZoomStep = 25.0f;  // qt max mov per ogni scatto //50

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Zoom", meta = (ClampMin = "0.0"))
		float MinCameraDistance= 180.0f;//180

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Zoom", meta = (ClampMin = "0.0"))
		float MaxCameraDistance= 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Zoom", meta = (ClampMin = "0.0"))
		float WeaponMinCameraDistance = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Zoom", meta = (ClampMin = "0.0"))
		float WeaponEquipCameraDistance = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Zoom", meta = (ClampMin = "0.0"))
		float SaveNormalCameraDistance = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float WalkSpeed = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float RunSpeed = 450.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float FastRunSpeed = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float FastRunDelay = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Pickup")
		UAnimMontage* PickupMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Pickup")
		FName PickupHandSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Pickup")
		float MaxPickupStartSpeed;

		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* GunLoadSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* CrouchSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundInventory")
		USoundBase* InventoryOpenSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundInventory")
		USoundBase* InventoryCloseSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Combat")
		USoundBase* RollSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Combat")
		USoundBase* RollLandSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Combat")
		USoundBase* PlayerHitSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Combat")
		USoundBase* PlayerImpactSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Combat")
		USoundBase* PlayerDeathSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Landmine")
		USoundBase* LandmineHitSound;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Mana")
		float WalkManaDrainPerSecond;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Mana")
		float RunManaDrainPerSecond;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Mana")
		float FastRunManaDrainPerSecond;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Mana")
		float AirManaDrainPerSecond;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Mana")
		float JumpManaCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Mana")
		float DefaultManaRestorePercent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment|Backpack")
		 UStaticMeshComponent* EquippedBackpackComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment|Backpack")
		bool bBackpackEquipped;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Progresion|Mission",meta =(AllowPrivateAccess="true"))
		bool bUSBCollected;  // se recupero chiave

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<APistolWeapon>PistolWeaponClass; //contiene classe per creare pistola dove accetta solo ApistoWeapon in Bp (stampo)

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon")
		APistolWeapon* EquippedWeapon; // contiene pistola creato stampo

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName WeaponSocketName;

	//GRENADE//
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade")
		TSubclassOf<AActor>GrenadeHeldClass; //contiene classe per creare pistola dove accetta solo ApistoWeapon in Bp (stampo)

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Grenade")
		AActor* EquippedGrenadeHeld; //stanmpo granata

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade")
		FName GrenadeSocketName;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Grenade",meta=(AllowPrivateAccess="true"))
		bool bGrenadeEquipped;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade")
		UAnimMontage* GrenadeThrowMontage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Grenade", meta = (AllowPrivateAccess = "true"))
		bool bIsThrowingGrenade;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade")
		TSubclassOf<AThrownGrenade>ThrownGrenadeClass;

	
	//// sensori
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Landmine|Sensors")
		USphereComponent* LeftFootMineTrigger;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Landmine|Sensors")
		USphereComponent* RightFootMineTrigger;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Landmine|Animation")
		UAnimMontage* LandmineDeathMontage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Landmine|Animation")
		UAnimMontage* LandmineStunMontage;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Landmine|Animation",meta = (ClampMin = "0.1"))
		float LandmineStunDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|UI")
		TSubclassOf<UUserWidget> CrossHairWidgetClass; //stampo di CrossHair

	UPROPERTY(Transient)  //deve sparire durante il game
		UUserWidget* CrossHairWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|UI")
		bool bIsAiming;

	bool EnsureCrosshairWidget();
	bool CalculateAimTarget(FVector& OutTargetLocation,FHitResult& OutHitResult);// calcolo punto mondi nel centro cam
	void UpdateCrosshairFromWeapon();
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Aim", meta = (ClampMin = "1000.0", UImin = "1000.0", UIMax = "100000.0"))
		float AimTraceLength = 50000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Aim")
		FVector AimCameraSocketOffset = FVector(0.0f, 75.0f, 10.0f); // spostamento cam quando player mira
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Damage")
		UAnimMontage* PlayerHitReactMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Damage")
		UAnimMontage* GunfireDeathMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death|Menu")
		FName MainMenuLevelName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death|Menu", meta=(ClampMin ="0.1"))
		 float ReturnToMainMenuDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement|Roll")
		UAnimMontage* RollMontage;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement|Roll", meta=(ClampMin="0.1"))
		float RollPlayRate;
	

	FVector DefaultCameraSocketOffset = FVector::ZeroVector; // no modificato blueprint salva socket

	UPROPERTY(Transient)
		UWidget* CrosshairImageWidget = nullptr;

	FVector2D CrosshairBaseRenderTranslation= FVector2D::ZeroVector;

	FVector2D RestoreTranslation = FVector2D::ZeroVector;


};