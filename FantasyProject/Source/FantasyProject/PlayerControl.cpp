// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControl.h"
#include "InventoryComponent.h"
#include "AmmoComponent.h"
#include "InventoryMainWidget.h"
#include "PlayerStatsWidget.h"
#include "ItemPickup.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "PistolWeapon.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Widget.h"
#include "DrawDebugHelpers.h"  // linea di prova 
#include "Components/ArrowComponent.h"
#include "PauseMenuWidget.h"
#include "KeyDoor.h"
#include "HelicopterToExit.h"
#include "EnemySoldier.h"
#include "CollisionShape.h"  // per creare FCollisoonShape:MakeCapsula 


//#include "C:/Program Files/Epic Games/UE_5.1/Engine/Plugins/VirtualProduction/TextureShare/Source/TextureShareCore/Private/Module/TextureShareCoreLogDefines.h"


// Sets default values
APlayerControl::APlayerControl() :
	_Speed(0.f),
	_IWR(0.f),
	_isInAir(false),
	_isJumping(false),
	_wasInAir(false),
	_isForwardJump(false),
	_VerticalVelocity(0.f),
	_DeltaTime(0.f),
	_AirControl(0.2f),
	_GunLoadSoundPLayed(false),
	CapsuleRadius(35.0f),
	CapsuleHeight(90.0f),
	jumpVelocity(480.0f),
	MoveinputX(0.0f),
	MoveinputY(0.0f),
	isGunVisible(false),
	isCrouching(false),
	isFastRunning(false),
	isRunPressed(false),
	CurrentPickup(nullptr),
	PendingPickup(nullptr),
	CurrentDoor(nullptr),
	CurrentHelicopter(nullptr),
	bIsPickingUp(false),
	bPickupObjectAttachedToHand(false),
	bIsDead(false),
	bIsStunned(false),
	bIsRolling(false),
	bInventoryVisible(false),
	bRestoreWeaponAfterPickup(false),
	bRestoreWaponAfetrGranade(false),
	bGmaeplayHUDVisible(false),
	InventoryComponent(nullptr),
	AmmoComponent(nullptr),
	InventoryWidgetClass(nullptr),
	PlayerStatsWidgetClass(nullptr),
	InventoryWidget(nullptr),
	PlayerStatsWidget(nullptr),
	PauseMenuWidget(nullptr),
	MaxHealth(100.f),
	Health(100.f),
	MaxMana(100.f),
	Mana(100.f),
	PivotCamera(nullptr),
	CamPlayerOne(nullptr),
	mouseSpeedX(0.15f),
	mouseSpeedY(0.15f),
	PickupMontage(nullptr),
	PickupHandSocketName(TEXT("RightHandPickupSocket")),
	MaxPickupStartSpeed(10.0f),
	GunLoadSound(nullptr),
	CrouchSound(nullptr),
	InventoryOpenSound(nullptr),
	InventoryCloseSound(nullptr),
	RollSound(nullptr),
	RollLandSound(nullptr),
	PlayerHitSound(nullptr),
	PlayerImpactSound(nullptr),
	PlayerDeathSound(nullptr),
	LandmineHitSound(nullptr),
	WalkManaDrainPerSecond(0.6f),
	RunManaDrainPerSecond(1.5f),
	FastRunManaDrainPerSecond(3.0f),
	AirManaDrainPerSecond(2.5f),
	JumpManaCost(4.0f),
	DefaultManaRestorePercent(25.0f),
	bBackpackEquipped(false),
	bUSBCollected(false),
	EquippedWeapon(nullptr),
	WeaponSocketName(TEXT("WeaponSocket_R")),
	EquippedGrenadeHeld(nullptr),
	GrenadeSocketName(TEXT("GrenadeSocket_R")),
	bGrenadeEquipped(false),
	GrenadeThrowMontage(nullptr),
	bIsThrowingGrenade(false),
	LandmineDeathMontage(nullptr),
	LandmineStunMontage(nullptr),
	LandmineStunDuration(2.5f),
	CrossHairWidgetClass(nullptr),
	CrossHairWidget(nullptr),
	bIsAiming(false),
	PlayerHitReactMontage(nullptr),
	GunfireDeathMontage(nullptr),
	MainMenuLevelName(TEXT("I_MainMenu")),
	ReturnToMainMenuDelay(4.0f),
	RollMontage(nullptr),
	RollPlayRate(1.5f)



{
	JumpMaxHoldTime = 0.0f;
	

	PrimaryActorTick.bCanEverTick = true;
	//Inventory
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));  //creazioni e possessione fisica dei  componente 

	//creazione dlel componente Ammo
	AmmoComponent = CreateDefaultSubobject<UAmmoComponent>(TEXT("AmmoComponent"));

	// radius
	
	GetCapsuleComponent()->SetCapsuleRadius(CapsuleRadius);
	
	GetCapsuleComponent()->SetCapsuleHalfHeight(CapsuleHeight);
	/** MOVE**/
	

	//Jump
	GetCharacterMovement()->JumpZVelocity=jumpVelocity;
	GetCharacterMovement()->AirControl = _AirControl;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	//ROT
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	/*SKELETON MESH and Transform
	/Script/Engine.SkeletalMesh'/Game/PROJECT/Actors/PlayerOne/PlayerOne.PlayerOne'*/
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("/Game/PROJECT/Actors/PlayerOne/PlayerOne.PlayerOne"));
	if (MeshObj.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshObj.Object);
		GetMesh()->SetRelativeScale3D(FVector(1.22f, 1.22f, 1.22f));
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -89.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}
	// sensor  mine
	LeftFootMineTrigger =CreateDefaultSubobject<USphereComponent>(TEXT("LeftFootMineTrigger"));

	LeftFootMineTrigger->SetupAttachment(GetMesh(),TEXT("LeftFoot"));

	LeftFootMineTrigger->InitSphereRadius(3.0f);

	LeftFootMineTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	LeftFootMineTrigger->SetCollisionObjectType(ECC_Pawn);

	LeftFootMineTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);

	LeftFootMineTrigger->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Overlap);

	LeftFootMineTrigger->SetGenerateOverlapEvents(true);

	LeftFootMineTrigger->ComponentTags.Add(TEXT("PlayerFoot"));


	RightFootMineTrigger =CreateDefaultSubobject<USphereComponent>(TEXT("RightFootMineTrigger"));

	RightFootMineTrigger->SetupAttachment(GetMesh(),TEXT("RightFoot"));

	RightFootMineTrigger->InitSphereRadius(3.0f);

	RightFootMineTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	RightFootMineTrigger->SetCollisionObjectType(ECC_Pawn);

	RightFootMineTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);

	RightFootMineTrigger->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Overlap);

	RightFootMineTrigger->SetGenerateOverlapEvents(true);

	RightFootMineTrigger->ComponentTags.Add(TEXT("PlayerFoot"));




	// Backpack
	EquippedBackpackComponent =CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EquippedBackpackComponent"));

	EquippedBackpackComponent->SetupAttachment(GetMesh(),TEXT("BackpackSocket"));

	EquippedBackpackComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);      

	EquippedBackpackComponent->SetGenerateOverlapEvents(false);  

	EquippedBackpackComponent->SetVisibility(false, true);    //visibility component

	//*    SPRING ARM    *//
	PivotCamera = CreateDefaultSubobject<USpringArmComponent>(TEXT("PivotCamera"));
	PivotCamera->SetupAttachment(RootComponent);
	/*Arm con Camera*/
	PivotCamera->bUsePawnControlRotation = true; //rotazione del controllo e mouse
	PivotCamera->bInheritPitch = true;
	PivotCamera->bInheritYaw = true;
	PivotCamera->bInheritRoll = false;
	
	//PivotCamera->TargetArmLength = 300.0f;
	/*no Collision*/
	PivotCamera->bDoCollisionTest = false;
	//*    CAMERA    *//
	CamPlayerOne = CreateDefaultSubobject<UCameraComponent>(TEXT("CamPlayerOne"));

	CamPlayerOne->SetupAttachment(PivotCamera,USpringArmComponent::SocketName);
	CamPlayerOne->bUsePawnControlRotation = false;
	CamPlayerOne->SetRelativeLocation(FVector::ZeroVector);
	CamPlayerOne->SetRelativeRotation(FRotator::ZeroRotator);
	


	/*AutoPossesion*/
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	
	/*ConstructorHelpers::FObjectFinder<UBlueprint>BP(TEXT("/Game/PROJECT/Actors/PlayerOne/AnimIstancePlayerOne.AnimIstancePlayerOne"));
	if (BP.Succeeded() && BP.Object)
	{
		GetMesh()->AnimClass = BP.Object->GeneratedClass;
	}*/
	ConstructorHelpers::FObjectFinder<USoundWave>GunLoadObj(TEXT("SoundWave'/Game/PROJECT/Audio/PistolSound/GunLoad.GunLoad'"));
	GunLoadSound = GunLoadObj.Object;
	// reference GunLoad               /Script/Engine.SoundWave'/Game/PROJECT/Audio/PistolSound/GunLoad.GunLoad'
	ConstructorHelpers::FObjectFinder<USoundWave>CrounchObj(TEXT("SoundWave'/Game/PROJECT/Audio/Crouch/Crouch.Crouch'"));
	CrouchSound = CrounchObj.Object;
	/*ConstructorHelpers::FObjectFinder<USoundWave>OpenInventory(TEXT("SoundWave'/Game/PROJECT/Audio/Inventory/OpenClose.OpenClose'"));
	InventoryOpenSound = OpenInventory.Object;
	ConstructorHelpers::FObjectFinder<USoundWave>CloseInventory(TEXT("SoundWave'/Game/PROJECT/Audio/Inventory/OpenClose.OpenClose'"));
	InventoryCloseSound = CloseInventory.Object; */
	
}




void APlayerControl::BeginPlay()
{
	Super::BeginPlay();

	// Salva una sola volta l'offset originale impostato della Camera.
	
	if (IsValid(PivotCamera))
	{
		DefaultCameraSocketOffset = PivotCamera->SocketOffset;

		PivotCamera->TargetArmLength = FMath::Clamp(WeaponEquipCameraDistance,WeaponMinCameraDistance,MaxCameraDistance);

		PivotCamera->SocketOffset = DefaultCameraSocketOffset;
	}

	bIsAiming = false;
	SetAimingRotationMode(false);

	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.AddDynamic(this,&APlayerControl::RefreshInventoryUI); 
	}
	if (AmmoComponent)																					//eventi inventario e munizioni
	{
		AmmoComponent->OnAmmoChanged.RemoveDynamic(this, &APlayerControl::RefreshAmmoUI);
		AmmoComponent->OnAmmoChanged.AddDynamic(this, &APlayerControl::RefreshAmmoUI);
	}

	CreateInventoryWidgetIfNeeded();
	CreatePlayerStatsWidgetIfNeeded();
	RefreshPlayerStatsUI();

	if (USkeletalMeshComponent* PlayerMesh = GetMesh())
	{
		if (UAnimInstance* AnimInstance = PlayerMesh->GetAnimInstance())
		{
			AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this,&APlayerControl::HandlePickupMontageNotifyBegin);

			AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this,&APlayerControl::HandlePickupMontageNotifyBegin);
		}
	}

	CreateAndAttachWeapon();

	if (EnsureCrosshairWidget() && IsValid(CrossHairWidget))
	{
		CrossHairWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}
float APlayerControl::TakeDamage(float DamageAmount,const FDamageEvent& DamageEvent,AController* EventInstigator,AActor* DamageCauser)
{
	if (bIsDead || DamageAmount <= 0.0f)
	{
		return 0.0f;  // non applica danni negativi dopo la morte
	}

	const float AppliedDamage =Super::TakeDamage(DamageAmount,DamageEvent,EventInstigator,DamageCauser);

	if (AppliedDamage <= 0.0f)
	{
		return 0.0f;
	}

	Health = FMath::Clamp(Health - AppliedDamage,0.0f,MaxHealth);

	RefreshPlayerStatsUI();

	UE_LOG(LogTemp, Warning, TEXT("Colpito: danno %.1f | vita %.1f / %.1f | causa: %s"),AppliedDamage,Health,MaxHealth,*GetNameSafe(DamageCauser));

	if (Health <= KINDA_SMALL_NUMBER)  // colpo letale proiettile
	{
		// Riutilizza per ora la morte già presente nel progetto.
		//DieFromLandmine(); 
		DieFromGunFire();
		return AppliedDamage;
	}

	if (PlayerHitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PlayerHitSound, GetActorLocation());
		
	}
	if (PlayerImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PlayerImpactSound, GetActorLocation());
	}
	if (PlayerHitReactMontage)
	{
		const float MontageDuration = PlayAnimMontage(PlayerHitReactMontage,1.0f);
		if (MontageDuration <= 0.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerHitReactMontage non riprodotto: controlla Skeleton e Slot"));
		}
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerHitReactMontage non assegnato in BP_PlayerWoman"));
	}
	if (PlayerStatsWidget)
	{
		const FString Message = FString::Printf(TEXT("Colpito: -%.0f vita"), AppliedDamage);
		PlayerStatsWidget->ShowNotificationMessage(FText::FromString(Message), EPlayerNotificationType::Warning, 1.5f);
	}

	return AppliedDamage;
}


// Called every frame
void APlayerControl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	_DeltaTime = DeltaTime;

	//Velocity
	const FVector Fv= GetVelocity();
	//float _speed = speed.Size(); //modulo vettore
	//varWalk = (_speed > 0) ? true : false;
	_Speed = Fv.Size2D();
	_VerticalVelocity = Fv.Z;
	
	const bool bCurrentlyInAir= GetCharacterMovement()->IsFalling();
	_isInAir = bCurrentlyInAir;
	if (!_wasInAir && bCurrentlyInAir) // saltato/ caduto
	{
		FStopAim(); //imposta asse orginale
		
	}
	if (bCurrentlyInAir)
	{
		_isJumping = false;
	}
	if (_wasInAir && !bCurrentlyInAir) // appena atterrato
	{
		//_isJumping = false;
		_isForwardJump = false;
		_VerticalVelocity = 0.f;
		
		if (isGunVisible && !isCrouching)
		{
			FStartAim();
		}
	}
	_wasInAir = bCurrentlyInAir;
	UpdateIWR();
	
	ApplyMovementManaDrain(DeltaTime);
	

}


void APlayerControl::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	/*WASD*/
	PlayerInputComponent->BindAxis("MoveToX", this, &APlayerControl::FMoveToX);
	PlayerInputComponent->BindAxis("MoveToY", this, &APlayerControl::FMoveToY);
	/*ROTATION_MOUSE*/
	PlayerInputComponent->BindAxis("RotMouseX", this, &APlayerControl::FRotMouseX);
	PlayerInputComponent->BindAxis("LookUpDown", this, &APlayerControl::FLookUpDown);
	PlayerInputComponent->BindAxis("Scroll", this, &APlayerControl::FScroll);
	/*RUN*/
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &APlayerControl::FRun);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &APlayerControl::FNotRun);
	/*JUMP*/
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerControl::FJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerControl::FNotJump);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerControl::FToggleCrouch);
	// ROLL//
	PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &APlayerControl::FRoll);
	PlayerInputComponent->BindAction("Gun", IE_Pressed, this, &APlayerControl::FGun);
	// INVENTORY//
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerControl::FInteract);
	PlayerInputComponent->BindAction("UseItem", IE_Pressed, this, &APlayerControl::FUseItem);
	PlayerInputComponent->BindAction("FToggleInventory", IE_Pressed, this, &APlayerControl::FToggleInventory);
	// FIRE//
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerControl::FFire);
	//GRENADE//
	PlayerInputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &APlayerControl::FThrowGrenade);
	FInputActionBinding& PauseBinding =PlayerInputComponent->BindAction("PauseMenu",IE_Pressed,this,&APlayerControl::TogglePauseMenu);

	PauseBinding.bExecuteWhenPaused = true;


}
bool APlayerControl::IsWallBlockingMovement(const FVector& Direction) const  
{
	UWorld* World = GetWorld(); //mondo gico per controloo
	const UCapsuleComponent* Capsule = GetCapsuleComponent(); // capsula player 

	if (!World || !Capsule)
	{
		return false;
	}

	FVector FlatDirection(Direction.X, Direction.Y, 0.0f); //xyz WS/DA/Z=0 CONTROLLO MOV hori

	if (!FlatDirection.Normalize())  //vettore a 1
	{
		return false;
	}

	const FVector Start = Capsule->GetComponentLocation();//centro capsula
	const FVector End = Start + FlatDirection * 5.0f;//10 cm.davant alla capsula daregolare

	// Raggio Riduco leggermente la capsula per non rilevare il pavimento.
	const float Radius =FMath::Max(1.0f, Capsule->GetScaledCapsuleRadius() - 2.0f);
	//meta altezza e riduco evitando che rilevi sempre il floor
	const float HalfHeight =FMath::Max(Radius, Capsule->GetScaledCapsuleHalfHeight() - 2.0f);

	const FCollisionShape CapsuleShape =FCollisionShape::MakeCapsule(Radius, HalfHeight); // creazioen capsula e volume

	FCollisionQueryParams Params(SCENE_QUERY_STAT(PlayerWallSweep),false,this);//Diagnostica /collisone sempliec false e ignora player

	FHitResult Hit;
																			//rotazione capsula //coll blocco Pawn 
	const bool bHit = World->SweepSingleByChannel(Hit,Start,End,Capsule->GetComponentQuat(),ECC_Pawn,CapsuleShape,Params);

	if (!bHit || !Hit.IsValidBlockingHit())
	{
		return false;
	}

	// Evita di considerare il pavimento o una normale salita.
	const bool bIsWall =FMath::Abs(Hit.ImpactNormal.Z) < 0.35f;  // ditizione mutro pavimento verticale no blooco player

	const FVector WallNormal2D(Hit.ImpactNormal.X,Hit.ImpactNormal.Y,0.0f);  // normale hori del muro

	const bool bMovingIntoWall =FVector::DotProduct(FlatDirection,WallNormal2D.GetSafeNormal()) < -0.05f;// doc productoir confronta due direioni uguali 1,0(perpendicolari,-1(opposte)punta muto

	return bIsWall && bMovingIntoWall;  // true se supeeficie è muro e player sta andando dentro al muro
}
/*void APlayerControl::FMoveToX(float value)
{
	MoveinputX = value;

	if (bIsDead || bIsStunned || bIsRolling || bIsThrowingGrenade)
	{
		return;
	}
	if (!isGunVisible)
	{
		if (UCharacterMovementComponent* Movement = GetCharacterMovement())
		{
			Movement->bOrientRotationToMovement = false;
			Movement->bUseControllerDesiredRotation = true;
			Movement->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
		}
	}
	//AddMovementInput(GetActorForwardVector(), value);
	if (Controller && value != 0.0f)
	{
		FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRot(0.f, Rotation.Yaw, 0.f);
		FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, value);
	}

}*/
void APlayerControl::FMoveToX(float value)
{
	MoveinputX = value;

	if (bIsDead || bIsStunned || bIsRolling || bIsThrowingGrenade)
	{
		return;
	}

	if (!isGunVisible)
	{
		if (UCharacterMovementComponent* Movement = GetCharacterMovement())
		{
			Movement->bOrientRotationToMovement = false;
			Movement->bUseControllerDesiredRotation = true;
			Movement->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
		}
	}

	if (Controller && !FMath::IsNearlyZero(value))
	{
		const FRotator Rotation = Controller->GetControlRotation();  // rot telecamera 
		const FRotator YawRot(0.0f, Rotation.Yaw, 0.0f);// usa solo hori yaw no pitch (basso) e roll inclinazione

		const FVector Direction =FRotationMatrix(YawRot).GetUnitAxis(EAxis::X); // conversione tor in vettore

		const FVector ActualDirection =Direction * FMath::Sign(value); // direzione reale w o S 1- 1

		if (IsWallBlockingMovement(ActualDirection))
		{
			if (UCharacterMovementComponent* Movement = GetCharacterMovement())
			{
				Movement->StopMovementImmediately();
			}

			return;
		}

		AddMovementInput(Direction, value);//regola il Charactermovememt
	}
}
/*void APlayerControl::FMoveToY(float value)
{
	MoveinputY = value;
	if (bIsDead || bIsStunned || bIsRolling || bIsThrowingGrenade)
	{
		return;
	}
	if (!isGunVisible)
	{
		if (UCharacterMovementComponent* Movement = GetCharacterMovement())
		{
			Movement->bOrientRotationToMovement = false;
			Movement->bUseControllerDesiredRotation = true;
			Movement->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
		}
	}
	//AddMovementInput(GetActorRightVector(), value);
	if (Controller && value != 0.0f)
	{
		FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRot(0.f, Rotation.Yaw, 0.f);
		FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, value);
	}
}*/

void APlayerControl::FMoveToY(float value)  //AD 1 -1  0
{
	MoveinputY = value;

	if (bIsDead || bIsStunned || bIsRolling || bIsThrowingGrenade)
	{
		return;
	}

	if (!isGunVisible)
	{
		if (UCharacterMovementComponent* Movement = GetCharacterMovement())
		{
			Movement->bOrientRotationToMovement = false;
			Movement->bUseControllerDesiredRotation = true;
			Movement->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
		}
	}

	if (Controller && !FMath::IsNearlyZero(value))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRot(0.0f, Rotation.Yaw, 0.0f);

		const FVector ForwardDirection =
			FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);  // asse x

		const FVector RightDirection =FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y); //assey 

		// Se W oppure S sta spingendo contro un muro,
		// blocca anche il movimento laterale A/D.
		if (!FMath::IsNearlyZero(MoveinputX))  // control mOVEINPUT x
		{
			const FVector ActualForwardDirection =ForwardDirection * FMath::Sign(MoveinputX);

			if (IsWallBlockingMovement(ActualForwardDirection))
			{
				if (UCharacterMovementComponent* Movement =GetCharacterMovement())
				{
					Movement->StopMovementImmediately();
				}

				return;
			}
		}

		// Controlla anche un muro direttamente a destra o sinistra.
		const FVector ActualRightDirection =RightDirection * FMath::Sign(value);

		if (IsWallBlockingMovement(ActualRightDirection))  // PERMETTE IL MOVE LATERLA
		{
			if (UCharacterMovementComponent* Movement =
				GetCharacterMovement())
			{
				Movement->StopMovementImmediately();
			}

			return;
		}

		AddMovementInput(RightDirection, value);
	}
}
void APlayerControl::FRun()
{
	if(bIsDead || bIsStunned || bIsPickingUp || bIsRolling)
	{
		return;
	}

	/*if (bIsPickingUp)
	{
		return;
	}*/
	if (_isInAir)
	{
		return;
	}
	if (!HasUsableMana())
	{
		ForceWalkBecauseManaIsEmpty();
		/*
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1001, 2.5f, FColor::Red, TEXT("Mana insufficiente per correre."));
		} */
		if (PlayerStatsWidget)
		{
			PlayerStatsWidget->ShowWarningMessage(FText::FromString(TEXT("Mana Insufficiente per correre.")));
		}
		return;
	}
	isRunPressed = true;
	isFastRunning = false;
	ApplyMovementSpeed();
	GetWorldTimerManager().ClearTimer(FastRunTimerHandle);
	GetWorldTimerManager().SetTimer(FastRunTimerHandle, this, &APlayerControl::StartFastRun, FastRunDelay, false);
	
	
}
void APlayerControl::StartFastRun()
{
	if (bIsDead || bIsStunned || bIsRolling)
	{
		return;
	}

	if (!isRunPressed || _isInAir)
	{
		return;
	}
	if (!HasUsableMana())
	{
		ForceWalkBecauseManaIsEmpty();
		return;
	}
	isFastRunning = true;
	ApplyMovementSpeed();
	/*if (isGunVisible)
	{
		FStopAim();
	}*/

}

void APlayerControl::FNotRun()
{
	if (bIsDead || bIsStunned)
	{
		return;
	}
	//GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	//StopFastRun();
	isRunPressed = false;
	isFastRunning = false;
	GetWorldTimerManager().ClearTimer(FastRunTimerHandle);
	ApplyMovementSpeed(); //QUANDO RILASCIA TORNA A 300
	/*const bool bIsCurrentlyInAir = GetCharacterMovement() && GetCharacterMovement()->IsFalling();
	
	if (isGunVisible && !bIsCurrentlyInAir) 
	{
		FStartAim();
	}
	*/ 

}


/*void APlayerControl::StopFastRun()
{
	isFastRunning = false;
	GetWorldTimerManager().ClearTimer(FastRunTimerHandle);
	ApplyMovementSpeed();
} */
void APlayerControl::ApplyMovementSpeed() 
{
	if (isFastRunning)
	{
		GetCharacterMovement()->MaxWalkSpeed = FastRunSpeed;//600
	}
	else if(isRunPressed) //(isGunVisible || isCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed; //450
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;//300
	}
}


void APlayerControl::FJump()
{
	if(bIsDead || bIsStunned || bIsPickingUp || bIsRolling || bIsThrowingGrenade)
	{
		return;
	}

	
	if (isCrouching)
	{
		isCrouching = false;
		UpdateIWR();
		ApplyMovementSpeed();
		
	} 

	if (Mana < JumpManaCost)
	{
		ForceWalkBecauseManaIsEmpty();
		/*
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Mana Esaurita : No Jump."));
		}*/
		if (PlayerStatsWidget)
		{
			PlayerStatsWidget->ShowWarningMessage(FText::FromString(TEXT("Mana Insufficiente: No Salto.")));
		}
		return;
	}
	const UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (Movement && !GetCharacterMovement()->IsFalling() && !_isJumping)
	{
		const bool bisMoving = GetVelocity().Size2D() > 50.f;
		_isForwardJump = isRunPressed && bisMoving;
		_isJumping = true;
		FStopAim();
		Mana = FMath::Clamp(Mana - JumpManaCost, 0.0f, MaxMana);
		RefreshPlayerStatsUI();
		Jump();

	}
	
}
void APlayerControl::FNotJump()
{
	if (bIsDead || bIsStunned)
	{
		return;
	}
	StopJumping();
	
}

void APlayerControl::FToggleCrouch()
{
	if (bIsDead || bIsStunned || bIsPickingUp || bIsRolling)
	{
		return;
	}
	if (isCrouching)
	{
		FNotCrouch(); // sectrcl 
	}
	else
	{
		FCrouch();//no ctrl
	}
}

void APlayerControl::FCrouch()
{
	if(bIsDead || bIsStunned || bIsPickingUp || bIsRolling)
	{
		return;
	}

	isCrouching = true;
	//isGunVisible = true;
	PlayCrouchSound();
	UpdateIWR();
	ApplyMovementSpeed();
	if (isGunVisible && IsValid(EquippedWeapon))
	{
		FStartAim();
	}
	else
	{
		FStopAim();
	}
	
}
void APlayerControl::FNotCrouch()
{
	if (bIsDead || bIsStunned|| bIsPickingUp || bIsRolling)
	{
		return;
	}
	isCrouching = false;
	UpdateIWR();
	ApplyMovementSpeed();
	if (isGunVisible && IsValid(EquippedWeapon))
	{
		FStartAim();
	}
	else
	{
		FStopAim();
	}
	
}


void APlayerControl::FGun()
{
	if (bIsDead || bIsStunned || bIsPickingUp || bIsRolling)
	{
		return;
	}

	// Mantiene il comportamento già presente:
	// non permette di riporre la pistola mentre il personaggio è accovacciato.
	if (isCrouching && isGunVisible)
	{
		return;
	}

	if (!IsValid(EquippedWeapon))
	{
		CreateAndAttachWeapon();
	}

	if (!IsValid(EquippedWeapon))
	{
		UE_LOG(LogTemp,Warning,TEXT("FGun: impossibile creare o recuperare EquippedWeapon."));

		return;
	}

	const bool bWillShowWeapon = !isGunVisible;

	if (bWillShowWeapon)
	{
		if (IsValid(PivotCamera))
		{
			// Memorizza la distanza della camera prima di estrarre la pistola.
			SaveNormalCameraDistance = FMath::Clamp(PivotCamera->TargetArmLength,MinCameraDistance,MaxCameraDistance);

			PivotCamera->TargetArmLength = FMath::Clamp(WeaponEquipCameraDistance,WeaponMinCameraDistance,MaxCameraDistance);
		}

		isGunVisible = true;
		EquippedWeapon->SetActorHiddenInGame(false);

		// FStartAim gestisce camera, rotazione e mirino.
		FStartAim();

		if (!_GunLoadSoundPLayed)
		{
			PlayGunLoadSound();
			_GunLoadSoundPLayed = true;
		}
	}
	else
	{
		// FStopAim ripristina camera, rotazione e mirino.
		FStopAim();

		isGunVisible = false;
		EquippedWeapon->SetActorHiddenInGame(true);

		if (IsValid(PivotCamera))
		{
			PivotCamera->TargetArmLength = FMath::Clamp(SaveNormalCameraDistance,MinCameraDistance,MaxCameraDistance);
		}

		_GunLoadSoundPLayed = false;
	}

	UpdateIWR();
	ApplyMovementSpeed();
}



void APlayerControl::PlayGunLoadSound() 
{
	if (GunLoadSound && GetWorld())
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), GunLoadSound, GetActorLocation());
	}

}

void APlayerControl::PlayCrouchSound()
{
	if (CrouchSound && GetWorld())
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), CrouchSound, GetActorLocation());
	}

}

void APlayerControl::UpdateIWR()
{
	// Crouch 

	if (isCrouching)
	{
		_IWR = 200.f;
		
	}
	
	
	else if (isGunVisible)
	{
		_IWR = 100.f;
	}
	else
	{
		_IWR = 0.f;
	}
	
	
}
/*ROT MOUSE*/
void APlayerControl::FRotMouseX(float Value)
{
	if (bIsDead || !Controller || FMath::IsNearlyZero(Value))
	{
		return;
	}
	AddControllerYawInput(Value * mouseSpeedX );
}
void APlayerControl::FLookUpDown(float Value)
{
	//AddControllerPitchInput(value * mouseSpeedY * _DeltaTime);
	if (bIsDead || !Controller || FMath::IsNearlyZero(Value)) // no control valido o mouse no mov stop
	{
		return;
	}  // modifica comp vertical con clamp;
	FRotator ControlRotation = Controller->GetControlRotation();
	const float CurrentPitch = FRotator::NormalizeAxis(ControlRotation.Pitch);
	const float PitchVariation = Value * mouseSpeedY;
	//const float ActiveMiniPicth = isGunVisible ? -60.f : MinCameraPitch;  //-50
	//const float ActivteMaxPitch = isGunVisible ? 40.0f : MaxCameraPitch;    //30
	//ControlRotation.Pitch = FMath::Clamp(CurrentPitch + PitchVariation, ActiveMiniPicth, ActivteMaxPitch);
	ControlRotation.Pitch = FMath::Clamp(CurrentPitch + PitchVariation, MinCameraPitch, MaxCameraPitch);
	ControlRotation.Roll = 0.0f;
	Controller->SetControlRotation(ControlRotation);
#if !UE_BUILD_SHIPPING
	if (GEngine)
	{
		const float DebugPitch =FRotator::NormalizeAxis(Controller->GetControlRotation().Pitch);
		  
		//** VERIFICA SETTAGGIO TOTAZIONE CAM **//
		//GEngine->AddOnScreenDebugMessage(1001,0.05f,FColor::Cyan,FString::Printf(TEXT("Camera Pitch: %.1f | Min: %.1f | Max: %.1f"),DebugPitch,MinCameraPitch,MaxCameraPitch));
	}
#endif 

}

void APlayerControl::FScroll(float value)
{
	if (!IsValid(PivotCamera) || FMath::IsNearlyZero(value))
	{
		return;
	}

	const float NewArmLength =PivotCamera->TargetArmLength - (value * CameraZoomStep);

	if (isGunVisible)
	{
		/*
		 * Con la pistola estratta:
		 * distanza minima = WeaponMinCameraDistance
		 * distanza massima = WeaponEquipCameraDistance
		 *
		 * avvicinare la camera, ma non puoi allontanarla oltre la posizione
		 * iniziale della modalità pistola.
		 */
		PivotCamera->TargetArmLength =FMath::Clamp(NewArmLength,WeaponMinCameraDistance,WeaponEquipCameraDistance);
	}
	else
	{
		/*
		 * Senza pistola utilizziamo il normale
		 * intervallo della telecamera.
		 */
		PivotCamera->TargetArmLength =FMath::Clamp(NewArmLength,MinCameraDistance,MaxCameraDistance);

		// Conserviamo anche gli spostamenti effettuati
		// nella modalità normale.
		//SaveNormalCameraDistance =PivotCamera->TargetArmLength;
	}

	
}


void APlayerControl::FRoll()
{
	if (bIsDead || bIsStunned || bIsPickingUp || bIsRolling)
	{
		return;
	}

	UCharacterMovementComponent* Movement = GetCharacterMovement();

	if (!Movement || GetCharacterMovement()->IsFalling())
	{
		return;
	}

	if (!RollMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Roll Montage non Assegnato."));

		return;
	}
	if (RollSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, RollSound, GetActorLocation());
	}

	if (isCrouching) // se crouch roll dopo crouch
	{
		FNotCrouch();
	}
	FVector RollDirection = GetActorForwardVector(); //direzione avanti player
	if (Controller)
	{
		const FRotator ControlRotation = Controller->GetControlRotation();  //prendi rot controller/cam

		const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);  //hori

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); //davanti rispetto a cam

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);// destra

		const FVector InputDirection = (ForwardDirection  * MoveinputX) + (MoveinputY  * RightDirection);

		// Se non è zreo Roll allora direzione 
		if (!InputDirection.IsNearlyZero())
		{
			RollDirection = InputDirection.GetSafeNormal(); // vettore normalizzato lenght 1
		}
		
	}
	// no mira prima del roll
	if (isGunVisible && bIsAiming && IsValid(EquippedWeapon))
	{
		SetAimingRotationMode(false);
	}
	else
	{
		FStopAim();
	}
	
	const bool bRollRight = FMath::RandBool();
	const float RollOffset = bRollRight ? 180.0F : 0.0f;  // o 90/-90

	const FRotator RollRotation(0.0f, RollDirection.Rotation().Yaw + RollOffset, 0.0f);

	SetActorRotation(RollRotation);
	

	Movement->StopMovementImmediately(); // ferma la velocita precedente e RootMotion Roll a spostare il Character per fare partire montage

	bIsRolling = true;
	const float MontageDuration = PlayAnimMontage(RollMontage, RollPlayRate);

	if (MontageDuration <= 0.0f)
	{
		bIsRolling = false;
		UE_LOG(LogTemp, Warning, TEXT("Roll Montage non riprodotto."));
		return;
	}
	GetWorldTimerManager().ClearTimer(RollTimerHandle);  // se esiste già timer cancellalo
	const float RollDuration = MontageDuration * 0.70f;

	GetWorldTimerManager().SetTimer(RollTimerHandle, this, &APlayerControl::EndRoll, RollDuration, false);
	
}

void APlayerControl::EndRoll()
{
	if (!bIsDead && !bIsStunned && RollLandSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, RollLandSound, GetActorLocation());
	}
	bIsRolling = false;
	if (isGunVisible && bIsAiming && !isCrouching && !bIsDead && IsValid(EquippedWeapon))
	{
		SetAimingRotationMode(true);
		//FStartAim();
	}
}

void APlayerControl::FInteract()
{
	if(bIsDead || bIsStunned || bIsPickingUp || bIsRolling)
	{
		return;
	}
	

	//Helicpoter
	if (IsValid(CurrentHelicopter))
	{
		if (!bUSBCollected)
		{
			if (PlayerStatsWidget)
			{
				PlayerStatsWidget->ShowNotificationMessage(FText::FromString(TEXT("Devi prima recuperare la USB.")),EPlayerNotificationType::Warning,2.5f);
			}

			return;
		}


		if (PlayerStatsWidget)
		{
			PlayerStatsWidget->HideInteractionPrompt();
		}


		UGameplayStatics::OpenLevel(this,MainMenuLevelName);

		return;
	}
	if (!IsValid(CurrentPickup))
	{
		if (PlayerStatsWidget)
		{
			PlayerStatsWidget->ShowNotificationMessage(FText::FromString(TEXT("Nessun Oggetto vicino")), EPlayerNotificationType::Error, 2.0f);
		}
		/*if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Red, TEXT("Nessun oggetto vicino."));
		}*/
		return;
	}
	
	const UCharacterMovementComponent* Movement = GetCharacterMovement();//Serve per la gestione del movimento
	const float HorizontalSpeed = GetVelocity().Size2D(); //controllo velocità mov
	if (!Movement ||Movement->IsFalling() || HorizontalSpeed > MaxPickupStartSpeed) // raccolta con velocità max
	{
		if (PlayerStatsWidget)
		{
			PlayerStatsWidget->ShowNotificationMessage(FText::FromString(TEXT("Fermati prima di raccogliere l'oggetto.")),EPlayerNotificationType::Warning,2.0f);
		}
		return;
	}
	if (!PickupMontage)
	{
		UE_LOG(LogTemp, Error, TEXT("PickupMontage non assegnato in BP_PlayerWoman."));
		if (PlayerStatsWidget)
		{
			PlayerStatsWidget->ShowNotificationMessage(FText::FromString(TEXT("Animazione di raccolta non configurata.")),EPlayerNotificationType::Error,2.5f);
		}
		return;
	}

	UAnimInstance* AnimInstance =GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;

	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("AnimInstance non disponibile durante la raccolta."));
		return;
	}
	PendingPickup = CurrentPickup; //Congela obj prima del ritardo anim.
	bIsPickingUp = true;
	HideWeaponForPickup();
	bPickupObjectAttachedToHand = false;

	if (PlayerStatsWidget)
	{
		PlayerStatsWidget->HideInteractionPrompt(); // nasconde
	}

	isRunPressed = false; //no run
	isFastRunning = false; 
	GetWorldTimerManager().ClearTimer(FastRunTimerHandle);//annulla timer corsa

	if (UCharacterMovementComponent* MovementToLock = GetCharacterMovement())  //annulla mov
	{
		MovementToLock->StopMovementImmediately();
		MovementToLock->DisableMovement();
	}

	if (APlayerController* PlayerController =Cast<APlayerController>(GetController()))
	{
		PlayerController->SetIgnoreMoveInput(true);
	}

	const float MontageDuration =AnimInstance->Montage_Play(PickupMontage, 1.0f);

	if (MontageDuration <= 0.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("Impossibile riprodurre PickupMontage."));
		RestoreMovementAfterPickup();

		if (PlayerStatsWidget)
		{
			PlayerStatsWidget->ShowNotificationMessage(FText::FromString(TEXT("Impossibile avviare la raccolta.")),EPlayerNotificationType::Error,2.5f);
		}
		return;
	}

	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindUObject(this,&APlayerControl::HandlePickupMontageEnded);
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate,PickupMontage);
}

void APlayerControl::HandlePickupMontageNotifyBegin(FName NotifyName,const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (NotifyName == FName(TEXT("GrenadeRelease")))
	{
		UE_LOG(LogTemp, Warning,TEXT("NOTIFY GRENADE RELEASE RICEVUTO"));

		// Direzione in cui guarda il Player/camera.
		FVector ThrowDirection =GetControlRotation().Vector();

		// Un po' verso l'alto per creare la parabola.
		ThrowDirection += FVector(0.0f, 0.0f, 0.25f);
		ThrowDirection.Normalize();


		// Punto di partenza = mano destra.
		FVector SpawnLocation =GetMesh()->GetSocketLocation(GrenadeSocketName);

		// La spostiamo leggermente davanti alla mano.
		SpawnLocation += ThrowDirection * 20.0f;

		const FRotator SpawnRotation =ThrowDirection.Rotation();


		if (ThrownGrenadeClass)
		{
			FActorSpawnParameters SpawnParameters;

			SpawnParameters.Owner = this;
			SpawnParameters.Instigator = this;

			SpawnParameters.SpawnCollisionHandlingOverride =ESpawnActorCollisionHandlingMethod::AlwaysSpawn;


			AThrownGrenade* ThrownGrenade =GetWorld()->SpawnActor<AThrownGrenade>(ThrownGrenadeClass,SpawnLocation,SpawnRotation,SpawnParameters);


			if (IsValid(ThrownGrenade))
			{
				ThrownGrenade->LaunchGrenade(ThrowDirection);
			}
		}


		// Elimina quella finta che avevamo nella mano.
		if (IsValid(EquippedGrenadeHeld))
		{
			EquippedGrenadeHeld->SetActorHiddenInGame(true);
			EquippedGrenadeHeld->Destroy();
			EquippedGrenadeHeld = nullptr;
		}

		// Non abbiamo più una granata equipaggiata.
		bGrenadeEquipped = false;

		return;
	}
	(void)BranchingPointPayload;

	if (!bIsPickingUp || !IsValid(PendingPickup))
	{
		return;
	}

	if (NotifyName == TEXT("PickupMoment"))
	{
		if (!bPickupObjectAttachedToHand)
		{
			bPickupObjectAttachedToHand =PendingPickup->AttachForPickup(GetMesh(), PickupHandSocketName);

			if (!bPickupObjectAttachedToHand)
			{
				UE_LOG(LogTemp,Error,TEXT("Impossibile attaccare l'oggetto al socket %s."),*PickupHandSocketName.ToString());
			}
		}

		return;
	}
	if (NotifyName == TEXT("PickupStoreMoment") && bPickupObjectAttachedToHand)
	{
		CompletePendingPickup();
		return;
	}

	/*if (NotifyName == TEXT("PickupStore") && bPickupObjectAttachedToHand)
	{
		CompletePendingPickup();
	}*/
}

void APlayerControl::CompletePendingPickup()
{
	if (!IsValid(PendingPickup)) // se non esiste obj valido da raccogliere ...return
	{
		bPickupObjectAttachedToHand = false;
		return;
	}

	AItemPickup* PickupToCollect = PendingPickup;
	const FInventoryItem CollectedItem = PickupToCollect->GetItemData();//copia dati del pickUp
	PendingPickup = nullptr;

	const bool bCollected =PickupToCollect->Pickup(this);

	if (bCollected)
	{
		if (CollectedItem.ItemType == EInventoryItemType::Equipment && CollectedItem.EquipmentSlot == EEquipmentSlot::Backpack)
		{
			EquipBackpack(); // se l'obj è lo zaino copia sulal sullo scheleton
		}
		// Missione
		if (CollectedItem.ItemID == FName(TEXT("MissionUSB")) && !bUSBCollected)
		{
			bUSBCollected = true;

			ShowMissionMessage(FText::FromString(TEXT("MISSIONE COMPIUTA - Raggiungi l'elicottero")), 6.0f);
		}
		if (CurrentPickup == PickupToCollect)
		{
			CurrentPickup = nullptr;
		}
	}
	else if (IsValid(PickupToCollect))
	{
		PickupToCollect->RestoreAfterPickupAttach();
		CurrentPickup = PickupToCollect;
	}

	bPickupObjectAttachedToHand = false;
}




void APlayerControl::HandlePickupMontageEnded(UAnimMontage* Montage,bool bInterrupted)
{
	(void)bInterrupted;

	if (Montage != PickupMontage)
	{
		return;
	}
	if (IsValid(PendingPickup) && bPickupObjectAttachedToHand)
	{
		CompletePendingPickup();
	}

	RestoreMovementAfterPickup();
}

void APlayerControl::RestoreMovementAfterPickup()
{
	if (APlayerController* PlayerController =Cast<APlayerController>(GetController()))
	{
		PlayerController->SetIgnoreMoveInput(false);
	}

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		if (Movement->MovementMode == MOVE_None)
		{
			Movement->SetMovementMode(MOVE_Walking);
		}
	}

	bIsPickingUp = false;
	bPickupObjectAttachedToHand = false;
	PendingPickup = nullptr;
	//ApplyMovementSpeed();
	RestoreWeaponAfterPickup();

	if (IsValid(CurrentPickup) && PlayerStatsWidget)
	{
		const FString Prompt = FString::Printf(TEXT("Premi E per raccogliere: %s"),*CurrentPickup->GetItemName().ToString());
		PlayerStatsWidget->ShowInteractionPrompt(FText::FromString(Prompt));
	}
}



void APlayerControl::FUseItem()  // Scorrimento obj
{
	if (bIsDead || bIsStunned || bIsPickingUp || bIsRolling)
	{
		return;
	}

	/*if (bIsPickingUp)
	{
		return;
	} */
	if (!InventoryComponent)
	{
		return;
	}

	const TArray<FInventoryItem>& Items = InventoryComponent->GetItems();
	for (const FInventoryItem& Item : Items)
	{
		if (Item.ItemType == EInventoryItemType::Consumable && Item.Quantity > 0)
		{
			UseConsumableItem(Item);
			return;
		}
	}
	/*if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Red, TEXT("Non hai consumabili da usare."));
	}*/
	if (PlayerStatsWidget)
	{
		PlayerStatsWidget->ShowNotificationMessage(FText::FromString(TEXT("Non hai consumabili da usare.")), EPlayerNotificationType::Warning, 2.5f);
	}
	
}

void APlayerControl::FToggleInventory()
{
	if (bIsDead || bIsStunned || bIsPickingUp || bIsRolling)
	{
		return;
	}

	/*if (bIsPickingUp)
	{
		return;
	}*/
	CreateInventoryWidgetIfNeeded();  // crea widget

	if (!InventoryWidget)
	{
		/*if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("InventoryWidgetClass non assegnata nel Player."));
		} */
		//(LogTemp, Error, TEXT("InventoryWidgetClass non assegnata nel Player.");
		UE_LOG(LogTemp, Error, TEXT("InventoryWidgetClass non assegnata nel Player."));
		if (PlayerStatsWidget)
		{
			PlayerStatsWidget->ShowNotificationMessage(FText::FromString(TEXT("Inventario non Disponibile.")), EPlayerNotificationType::Error, 3.0f);
		}
		return;
	}

	RefreshInventoryUI();  //Update

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (bInventoryVisible)
	{
		if (InventoryCloseSound)
		{
			UGameplayStatics::PlaySound2D(this, InventoryCloseSound);
		}
		InventoryWidget->RemoveFromParent();
		bInventoryVisible = false;
		//se pistola attiva repristino mirino
		if (isGunVisible && bIsAiming && IsValid(CrossHairWidget))
		{
			CrossHairWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		}

		if (PlayerController)
		{
			PlayerController->bShowMouseCursor = false;
			PlayerController->SetInputMode(FInputModeGameOnly());
		}
	}
	else
	{
		InventoryWidget->SetFilter(EInventoryFilter::All);
		RefreshInventoryUI();
		if (IsValid(CrossHairWidget))
		{
			CrossHairWidget->SetVisibility(ESlateVisibility::Hidden);
		}

		if (InventoryOpenSound)
		{
			UGameplayStatics::PlaySound2D(this, InventoryOpenSound);
		}
		InventoryWidget->AddToViewport();
		bInventoryVisible = true;

		if (PlayerController)
		{
			PlayerController->bShowMouseCursor = true;
			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputMode);
		}
	}
}
void APlayerControl::RefreshInventoryUI()
{
	if (InventoryWidget)
	{
		InventoryWidget->RefreshInventory();
	}
}

void APlayerControl::HandleUseItemFromUI(FInventoryItem ItemData)
{
	//UseConsumableItem(ItemData);
	if (!ItemData.IsValidItem())
	{
		return;
	}

	// CONSUMABILE
	if (ItemData.ItemType == EInventoryItemType::Consumable)
	{
		UseConsumableItem(ItemData);
		return;
	}

	// CHIAVE
	if (ItemData.ItemType == EInventoryItemType::KeyItem)
	{
		if (!CurrentDoor)
		{
			if (PlayerStatsWidget)
			{
				PlayerStatsWidget->ShowNotificationMessage(FText::FromString(TEXT("Devi Avvicinarti alla porta.")), EPlayerNotificationType::Warning, 2.0f);
			}
			return;

		}
		if (CurrentDoor->TryUseKey(ItemData.ItemID))
		{
			if (InventoryComponent)
			{
				InventoryComponent->RemoveItem(ItemData.ItemID, 1);
				RefreshInventoryUI();
			}
			CurrentDoor = nullptr;
			if (PlayerStatsWidget)
			{
				PlayerStatsWidget->HideInteractionPrompt();
				PlayerStatsWidget->ShowNotificationMessage(FText::FromString(TEXT("Porta Sbloccata")), EPlayerNotificationType::Success, 2.0f);
			}
			return;
		}
		if (PlayerStatsWidget)
		{
			PlayerStatsWidget->ShowNotificationMessage(FText::FromString(TEXT("Questa chiave non apre la porta")), EPlayerNotificationType::Error, 2.0f);
		}
		return;
		
		

		
	}
}

void APlayerControl::HandleEquipItemFromUI(FInventoryItem ItemData)
{
	if (!ItemData.IsValidItem())
	{
		return;
	}

	if (ItemData.ItemType != EInventoryItemType::Equipment)
	{
		return;
	}

	// GRANATA
	if (ItemData.EquipmentSlot == EEquipmentSlot::Grenade)
	{
		if (bGrenadeEquipped)
		{
			return;
		}
		if (!InventoryComponent || !InventoryComponent->HasItem(ItemData.ItemID, 1))
		{
			return;
		}
		
		EquipGrenade();
		if (!bGrenadeEquipped)
		{
			return;
		}

		InventoryComponent->RemoveItem(ItemData.ItemID, 1);
		RefreshInventoryUI();

		if (PlayerStatsWidget)
		{
			PlayerStatsWidget->ShowNotificationMessage(FText::FromString(TEXT("Granata equipaggiata")),EPlayerNotificationType::Success,2.0f);
		}

		return;
	}
	if (PlayerStatsWidget)
	{
		const FString Message = FString::Printf(TEXT("Equipaggiamento selezionato: %s"), *ItemData.ItemName.ToString());

		PlayerStatsWidget->ShowNotificationMessage(FText::FromString(Message), EPlayerNotificationType::Info, 3.0f);

	}
	/*if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, FString::Printf(TEXT("Equipaggiamento selezionato: %s. Lo colleghiamo nello step equip."), *ItemData.ItemName.ToString()));
	}*/
}
void APlayerControl::HandleCloseInventoryFromUI()
{
	if (bInventoryVisible)
	{
		FToggleInventory();
	}
}


void APlayerControl::CreateInventoryWidgetIfNeeded()
{
	if (InventoryWidget || !InventoryWidgetClass)
	{
		return;
	}

	InventoryWidget = CreateWidget<UInventoryMainWidget>(GetWorld(), InventoryWidgetClass);
	if (!InventoryWidget)
	{
		return;
	}

	InventoryWidget->SetInventoryComponent(InventoryComponent);
	InventoryWidget->OnUseItemRequested.AddDynamic(this, &APlayerControl::HandleUseItemFromUI);
	InventoryWidget->OnEquipItemRequested.AddDynamic(this, &APlayerControl::HandleEquipItemFromUI);
	InventoryWidget->OnCloseRequested.RemoveDynamic(this, &APlayerControl::HandleCloseInventoryFromUI);
	InventoryWidget->OnCloseRequested.AddDynamic(this, &APlayerControl::HandleCloseInventoryFromUI);
	RefreshInventoryUI();
}


bool APlayerControl::AddItemToInventory(const FInventoryItem& ItemData)
{
	if (!InventoryComponent)
	{
		return false;
	}

	const bool bAdded = InventoryComponent->AddItem(ItemData);
	if (bAdded && PlayerStatsWidget)   //(bAdded && GEngine)
	{
		const int32 TotalQuantity = InventoryComponent->GetItemQuantity(ItemData.ItemID);
		//GEngine->AddOnScreenDebugMessage(-1, 3.5f, FColor::Yellow, FString::Printf(TEXT("Inventario: %s x%d"), *ItemData.ItemName.ToString(), TotalQuantity));
		const FString Message = FString::Printf(TEXT("Raccolto: %s x%d"), *ItemData.ItemName.ToString(),TotalQuantity);
		PlayerStatsWidget->ShowNotificationMessage(FText::FromString(Message), EPlayerNotificationType::Success, 3.0f);
	}

	return bAdded;
}
bool APlayerControl::HasBackpackEquipped() const
{
	return bBackpackEquipped;
}

bool APlayerControl::HasMissionUSB() const
{
	return bUSBCollected;
}

void APlayerControl::ShowMissionMessage(const FText& Message, float Duration)
{
	CreatePlayerStatsWidgetIfNeeded();
	if (PlayerStatsWidget)
	{
		PlayerStatsWidget->ShowMissionMessage(Message, Duration);
	}
}

void APlayerControl::ShowProgressNotification(const FText& Message, float Duration)
{
	CreatePlayerStatsWidgetIfNeeded();
	if (PlayerStatsWidget)
	{
		PlayerStatsWidget->ShowNotificationMessage(Message, EPlayerNotificationType::Warning, Duration);
	}
}

void APlayerControl::SetCurrentPickup(AItemPickup* NewPickup)
{
	CurrentPickup = NewPickup;

	if(PlayerStatsWidget && CurrentPickup) //(GEngine && CurrentPickup)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Cyan, FString::Printf(TEXT("Premi E per raccogliere: %s"), *CurrentPickup->GetItemName().ToString()));
		const FString Prompt = FString::Printf(TEXT("Premi E per raccogliere: %s"), *CurrentPickup->GetItemName().ToString());
		PlayerStatsWidget->ShowInteractionPrompt(FText::FromString(Prompt));
	}
}

void APlayerControl::ClearCurrentPickup(AItemPickup* PickupToClear)
{
	if (CurrentPickup == PickupToClear)
	{
		CurrentPickup = nullptr;

		if (PlayerStatsWidget)
		{
			PlayerStatsWidget->HideInteractionPrompt();
		}

		/*if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Silver, TEXT("Oggetto fuori portata."));
		}*/
	}
}

UInventoryComponent* APlayerControl::GetInventoryComponent() const
{
	return InventoryComponent;
}

bool APlayerControl::UseConsumableItem(FInventoryItem ItemData)
{
	if (!InventoryComponent || !ItemData.IsValidItem() || !InventoryComponent->HasItem(ItemData.ItemID,1)) 
	{
		return false;
	}

	bool bUsed = false;

	if (ItemData.ConsumableEffect == EConsumableEffect::Health)
	{
		if (Health >= MaxHealth)
		{
			/* if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Red, TEXT("Vita gia' piena."));
			}*/
			if (PlayerStatsWidget)
			{
				PlayerStatsWidget->ShowNotificationMessage(FText::FromString(TEXT("La Vita è piena.")), EPlayerNotificationType::Warning, 2.5f);
			}
			return false;
		}

		RestoreHealth(ItemData.HealthAmount);
		bUsed = true;
	}
	else if (ItemData.ConsumableEffect == EConsumableEffect::Mana)
	{
		if (Mana >= MaxMana)
		{
			/*if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Red, TEXT("Mana gia' piena."));
			}*/
			if (PlayerStatsWidget)
			{
				PlayerStatsWidget->ShowNotificationMessage(FText::FromString(TEXT("Mana è già piena.")), EPlayerNotificationType::Warning, 2.5f);
			}
			return false;
		}
		const float RestorePercent = FMath::Clamp(DefaultManaRestorePercent, 0.0f, 100.0f);
		float ManaRestoreAmount = 0.0f;
		if (ItemData.ManaAmount > 0.0f)
		{
			ManaRestoreAmount = ItemData.ManaAmount;
		}
		else
		{
			ManaRestoreAmount = MaxMana * (RestorePercent / 100.0f);
		}
		//RestoreMana(ItemData.ManaAmount);
		RestoreMana(ManaRestoreAmount);
		bUsed = true;
	}
	else if (ItemData.ConsumableEffect == EConsumableEffect::Ammo)
	{
		if (!AmmoComponent)
		{
			return false;
		}
		if (AmmoComponent->IsFull())  // controllo munzione piene 
		{
			if (PlayerStatsWidget)
			{
				PlayerStatsWidget->ShowNotificationMessage(FText::FromString(TEXT("Munizioni sono al Massimo.")), EPlayerNotificationType::Warning, 2.5f);
			}
			return false;
		}
		if (ItemData.AmmoAmount <= 0)
		{
			if (PlayerStatsWidget)
			{
				PlayerStatsWidget->ShowNotificationMessage(FText::FromString(TEXT("Box non contiene munizioni configurate.")), EPlayerNotificationType::Error, 2.5f);
			}
			return false;
		}
		bUsed = AmmoComponent->AddAmmo(ItemData.AmmoAmount) > 0; //Aggiungi 
	}


	if (!bUsed)
	{
		if (PlayerStatsWidget)
		{
			PlayerStatsWidget->ShowNotificationMessage(FText::FromString(TEXT("Questo oggetto non si puo' usare ora.")), EPlayerNotificationType::Error, 2.5f);
		}
		/*if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Red, TEXT("Questo oggetto non si puo' usare ora."));
		} */
		return false;
	}

	InventoryComponent->RemoveItem(ItemData.ItemID, 1);

	if (ItemData.UseSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ItemData.UseSound, GetActorLocation());
	}

	/*/if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.5f, FColor::Green, FString::Printf(TEXT("Usato: %s | Vita %.0f/%.0f | Mana %.0f/%.0f"),
			*ItemData.ItemName.ToString(),
			Health,
			MaxHealth,
			Mana,
			MaxMana));
	} */
	if (PlayerStatsWidget)
	{
		const FString Message = FString::Printf(TEXT("Usato: %s"), *ItemData.ItemName.ToString());
		PlayerStatsWidget->ShowNotificationMessage(FText::FromString(Message), EPlayerNotificationType::Success, 3.0f);
	}
	return true;
}
void APlayerControl::RestoreHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	RefreshPlayerStatsUI();
}

void APlayerControl::RestoreMana(float Amount)
{
	Mana = FMath::Clamp(Mana + Amount, 0.0f, MaxMana);
	RefreshPlayerStatsUI();
	ApplyMovementSpeed();
}
void APlayerControl::CreatePlayerStatsWidgetIfNeeded()
{
	if (PlayerStatsWidget || !PlayerStatsWidgetClass)
	{
		return;
	}

	PlayerStatsWidget = CreateWidget<UPlayerStatsWidget>(GetWorld(), PlayerStatsWidgetClass);

	if (PlayerStatsWidget)
	{
		PlayerStatsWidget ->AddToViewport();
		PlayerStatsWidget->SetVisibility(ESlateVisibility::Hidden);
		RefreshPlayerStatsUI();
	}
}

void APlayerControl::RefreshPlayerStatsUI()
{
	if (PlayerStatsWidget)
	{
		PlayerStatsWidget->SetStats(Health, MaxHealth, Mana, MaxMana);
		if (AmmoComponent)
		{
			PlayerStatsWidget->SetAmmo(AmmoComponent->GetCurrentAmmo(), AmmoComponent->GetMaxAmmo());
		}
	}
}

void APlayerControl::RefreshAmmoUI(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (PlayerStatsWidget)
	{
		PlayerStatsWidget->SetAmmo(CurrentAmmo, MaxAmmo); //Aggiorna il numero dopo ogni ricarica
	}
}


bool APlayerControl::HasUsableMana() const
{
	return Mana > 0.0f;
}

void APlayerControl::ForceWalkBecauseManaIsEmpty()
{
	isRunPressed = false;
	isFastRunning = false;
	GetWorldTimerManager().ClearTimer(FastRunTimerHandle);

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

void APlayerControl::ApplyMovementManaDrain(float DeltaTime)
{
	if (bIsDead || bIsStunned)
	{
		return;
	}
	if (Mana <= 0.0f)
	{
		ForceWalkBecauseManaIsEmpty();
		//RefreshPlayerStatsUI();
		return;
	}

	const float HorizontalSpeed = GetVelocity().Size2D();

	if (HorizontalSpeed <= 5.0f)
	{
		//RefreshPlayerStatsUI();
		return;
	}

	float DrainPerSecond = WalkManaDrainPerSecond;

	if (_isInAir || GetCharacterMovement()->IsFalling())
	{
		DrainPerSecond = AirManaDrainPerSecond;
	}
	else if (isFastRunning)
	{
		DrainPerSecond = FastRunManaDrainPerSecond;
	}
	else if (isRunPressed)
	{
		DrainPerSecond = RunManaDrainPerSecond;
	}

	Mana = FMath::Clamp(Mana - DrainPerSecond * DeltaTime, 0.0f, MaxMana);

	if (Mana <= 0.0f)
	{
		ForceWalkBecauseManaIsEmpty();
	}

	RefreshPlayerStatsUI();
}
void APlayerControl::EquipBackpack()
{
	if (!EquippedBackpackComponent)
	{
		UE_LOG(LogTemp,Error,TEXT("EquippedBackpackComponent non valido."));

		return;
	}

	if (!EquippedBackpackComponent->GetStaticMesh())
	{
		UE_LOG(LogTemp,Error,TEXT("Nessuna Static Mesh assegnata allo zaino."));

		return;
	}

	EquippedBackpackComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	EquippedBackpackComponent->SetVisibility(true, true);

	bBackpackEquipped = true;
}

bool APlayerControl::IsDead() const 
{
	return bIsDead;
}

void APlayerControl::ApplyLandmineExplosion(bool bLethalHit,float DamagePercent)
{
	if (bIsDead)
	{
		return;
	}

	const float SafeDamagePercent =FMath::Clamp(DamagePercent,0.0f,100.0f);

	if (bLethalHit)
	{
		Health = 0.0f;
		Mana = 0.0f;

		RefreshPlayerStatsUI();

		if (PlayerStatsWidget)
		{
			PlayerStatsWidget->ShowNotificationMessage(FText::FromString(TEXT("Mina: colpo letale.")),EPlayerNotificationType::Error,3.0f);
		}

		DieFromLandmine();
		return;
	}

	const float DamageRatio =SafeDamagePercent / 100.0f;

	Health = FMath::Clamp(Health - (MaxHealth * DamageRatio),0.0f,MaxHealth);

	Mana = FMath::Clamp(Mana - (MaxMana * DamageRatio),0.0f,MaxMana);

	RefreshPlayerStatsUI();

	if (Health <= KINDA_SMALL_NUMBER)
	{
		DieFromLandmine();
		return;
	}

	if (PlayerStatsWidget)
	{
		const FString Message =FString::Printf(TEXT("Mina: -%.0f%% Vita e Mana."),SafeDamagePercent);

		PlayerStatsWidget->ShowNotificationMessage(FText::FromString(Message),EPlayerNotificationType::Warning,3.0f);
	}
	if (LandmineHitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, LandmineHitSound, GetActorLocation());
	}

	StartLandmineStun();
}
void APlayerControl::LockMovmentForLadnmine()
{
	isRunPressed = false;
	isFastRunning = false;
	isCrouching = false;

	_isJumping = false;
	_isForwardJump = false;

	GetWorldTimerManager().ClearTimer(FastRunTimerHandle);

	StopJumping();

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();

		GetCharacterMovement()->DisableMovement();
	}

	if (APlayerController* PlayerController =Cast<APlayerController>(GetController()))
	{
		PlayerController->SetIgnoreMoveInput(true);
	}

	UpdateIWR();
}

void APlayerControl::StartLandmineStun()
{
	if (bIsDead)
	{
		return;
	}

	bIsStunned = true;
	FStopAim();

	LockMovmentForLadnmine();

	if (UAnimInstance* AnimInstance =GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Stop(0.10f);

		if (LandmineStunMontage)
		{
			AnimInstance->Montage_Play(LandmineStunMontage,1.0f);
		}
	}

	GetWorldTimerManager().ClearTimer(LandmineStunTimerHandle);

	GetWorldTimerManager().SetTimer(LandmineStunTimerHandle,this,&APlayerControl::EndLandmineStun,FMath::Max(LandmineStunDuration,0.10f),false);
}

void APlayerControl::EndLandmineStun()
{
	if (bIsDead)
	{
		return;
	}

	bIsStunned = false;

	if (APlayerController* PlayerController =Cast<APlayerController>(GetController()))
	{
		PlayerController->SetIgnoreMoveInput(false);
	}

	if (GetCharacterMovement())
	{
		if (GetCharacterMovement()->MovementMode ==MOVE_None)
		{
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
	}

	ApplyMovementSpeed();
	UpdateIWR();

	if (isGunVisible && IsValid(EquippedWeapon))
	{
		FStartAim();
	}
	

}

void APlayerControl::DieFromLandmine()
{
	if (bIsDead)
	{
		return;
	}
	Mana = 0.0f;
	EnterDeathState(LandmineDeathMontage);
}

void APlayerControl::DieFromGunFire()
{
	if (bIsDead)
	{
		return;
	}
	EnterDeathState(GunfireDeathMontage);
}

void APlayerControl::EnterDeathState(UAnimMontage* DeathMontageToPlay)
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;
	bIsStunned = false;
	if (PlayerDeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PlayerDeathSound, GetActorLocation());
	}
	

	//FStopAim();
	bIsAiming = false;

	if (IsValid(CrossHairWidget))
	{
		CrossHairWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	Health = 0.0f;

	GetWorldTimerManager().ClearTimer(LandmineStunTimerHandle);

	RefreshPlayerStatsUI();

	
	LockMovmentForLadnmine();
	

	GetWorldTimerManager().ClearTimer(ReturnToMainMenuTimerHandle);  // timer e set per return Main Menu

	GetWorldTimerManager().SetTimer(ReturnToMainMenuTimerHandle, this, &APlayerControl::ReturnToMainMenuAfterDeath, FMath::Max(ReturnToMainMenuDelay, 0.1f), false);

	USkeletalMeshComponent* PlayerMesh = GetMesh();

	if (!PlayerMesh)
	{
		UE_LOG(LogTemp,Error,TEXT("Morte player: Skeletal Mesh non valida."));

		return;
	}

	//Disablita capsula collide corpo fisico  ------------NON VA BENE TESTATO IL PLAYER SI RITORCE -----
	/*if (UCapsuleComponent* PlayerCapsule = GetCapsuleComponent())
	{
		PlayerMesh->SetCollisionProfileName(TEXT("Ragdoll")); imposta collision
		PlayerMesh->SetAllBodiesSimulatePhysics(true); simula tutte le ossa
		PlayerMesh->SetSimulatePhysics(true); attiva la fiscia 
		PlayerMesh->WakeAllRigidBodies();//corpsi fiici sveglua
		PlayerMesh->bBlendPhysics = true;  /7usa mov fisico
		return;

	}*/

	UAnimInstance* AnimInstance =PlayerMesh->GetAnimInstance();

	if (!AnimInstance)
	{
		UE_LOG(LogTemp,Error,TEXT("Morte player: AnimInstance non valida."));

		return;
	}

	// Ferma Hit React, raccolta o altre animazioni attive.
	AnimInstance->Montage_Stop(0.10f);

	if (!DeathMontageToPlay)
	{
		UE_LOG(LogTemp,Warning,TEXT("Morte player: Death Montage non assegnato."));

		return;
	}

	const float MontageDuration =AnimInstance->Montage_Play(DeathMontageToPlay,1.0f);

	if (MontageDuration <= 0.0f)
	{
		UE_LOG(LogTemp,Warning,TEXT("Death Montage non riprodotto: controlla Skeleton e DefaultSlot."));
	}
}

void APlayerControl::ReturnToMainMenuAfterDeath()
{
	if (MainMenuLevelName.IsNone())
	{
		UE_LOG(LogTemp, Error, TEXT("Ritorno al menu fallito : MainMenuLevelname non configurato"));
		return;

	}

	UGameplayStatics::OpenLevel(this, MainMenuLevelName);
}







void APlayerControl::CreateAndAttachWeapon()  // quale pistola creare ->crea->collega alla mano->
{
	
	if (IsValid(EquippedWeapon))
	{
		return; // se esiste return
	}

	EquippedWeapon = nullptr;

	if (!PistolWeaponClass)  // verifica classe
	{
		UE_LOG(LogTemp,Error,TEXT("PlayerControl: PistolWeaponClass non assegnata. ""Apri BP_PlayerWoman e assegna BP_Pistol."));

		return;
	}
	// recupero mondo gioco e mesh player
	UWorld* World = GetWorld(); //genera actor
	USkeletalMeshComponent* CharacterMesh = GetMesh(); // skeletal

	if (!World || !CharacterMesh)
	{
		UE_LOG(LogTemp,Error,TEXT("PlayerControl: World o CharacterMesh non valido."));

		return;  
	}

	if (WeaponSocketName.IsNone())
	{
		UE_LOG(LogTemp,Error,TEXT("PlayerControl: WeaponSocketName non configurato."));

		return;
	}

	if (!CharacterMesh->DoesSocketExist(WeaponSocketName))
	{
		UE_LOG(LogTemp,Error,TEXT("PlayerControl: socket %s non trovato sullo Skeleton."),*WeaponSocketName.ToString());

		return;
	}

	FActorSpawnParameters SpawnParameters; // creazione pistola 

	SpawnParameters.Owner = this; //owner al player
	SpawnParameters.Instigator = this;  // chi causa l'azione dell'arma

	SpawnParameters.SpawnCollisionHandlingOverride =ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // collision durante lo spawn

	EquippedWeapon = World->SpawnActor<APistolWeapon>(PistolWeaponClass,FTransform::Identity,SpawnParameters); // creazione arma 

	if (!IsValid(EquippedWeapon))
	{
		EquippedWeapon = nullptr;

		UE_LOG(LogTemp, Error, TEXT("PlayerControl: creazione della pistola fallita."));

		return;
	}
	


	/*const bool bAttached = EquippedWeapon->AttachToComponent(CharacterMesh,FAttachmentTransformRules::SnapToTargetNotIncludingScale,WeaponSocketName); // collegamento alla mano al socket
	//SnapToTargetNotIncludingScale copia solo posizione e rotazione del socket no personaggio

	if (!bAttached)
	{
		UE_LOG(LogTemp,Error,TEXT("PlayerControl: collegamento della pistola ""al socket %s fallito."),	*WeaponSocketName.ToString());

		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;
		return;
	} */

	EquippedWeapon->AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);

	// azzeramento del tranform

	EquippedWeapon->SetActorRelativeLocation(FVector::ZeroVector);
	EquippedWeapon->SetActorRelativeRotation(FRotator::ZeroRotator);
	EquippedWeapon->SetActorRelativeScale3D(FVector::OneVector);

	EquippedWeapon->SetActorEnableCollision(false); // no collison per impdire urto al player block movcapsula palyer etc.

	// All'inizio segue lo stato corrente di isGunVisible.
	EquippedWeapon->SetActorHiddenInGame(!isGunVisible);
	UE_LOG(LogTemp,Log,TEXT("PlayerControl: pistola collegata a %s."),	*WeaponSocketName.ToString());
}
	
void APlayerControl::HideWeaponForPickup()
{
		// Ricorda se l'arma era realmente estratta prima della raccolta
	bRestoreWeaponAfterPickup =isGunVisible && IsValid(EquippedWeapon);
	FStopAim();

// Durante la raccolta l'arma deve essere nascosta.
	if (IsValid(EquippedWeapon))
	{
		EquippedWeapon->SetActorHiddenInGame(true);
	}
	// L'Animation Blueprint passa temporaneamente allo stato senza arma.
	isGunVisible = false;
	UpdateIWR();
	ApplyMovementSpeed();
}
void APlayerControl::RestoreWeaponAfterPickup()
{
	const bool bShouldShowWeapon =bRestoreWeaponAfterPickup && IsValid(EquippedWeapon) && !bIsDead && !bIsStunned;

	isGunVisible = bShouldShowWeapon;

	if (IsValid(EquippedWeapon))
	{
		EquippedWeapon->SetActorHiddenInGame(!bShouldShowWeapon);
	}

	bRestoreWeaponAfterPickup = false;

	UpdateIWR();
	ApplyMovementSpeed();
	if (bShouldShowWeapon)
	{
		FStartAim();  //control crocuh e jump nel mirino
	}
	else
	{
		FStopAim();
	}


}


void APlayerControl::FStartAim()   //mira
{
	const UCharacterMovementComponent* Movement =GetCharacterMovement();

	const bool bIsCurrentlyInAir =Movement && Movement->IsFalling();

	if (bIsDead ||bIsStunned ||bIsPickingUp || bIsRolling ||bIsCurrentlyInAir ||!isGunVisible ||!IsValid(EquippedWeapon))
	{
		return;
	}

	bIsAiming = true;

	// Fa ruotare il personaggio verso la direzione della camera.
	SetAimingRotationMode(true);

	if (IsValid(PivotCamera))
	{
		// AimCameraSocketOffset è uno spostamento relativo,
		// quindi deve essere sommato all'offset originale.
		PivotCamera->SocketOffset =DefaultCameraSocketOffset + AimCameraSocketOffset;
	}

	// Un problema del widget non deve impedire al giocatore di mirare.
	if (bGmaeplayHUDVisible && EnsureCrosshairWidget() && IsValid(CrossHairWidget))
	{
		CrossHairWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void APlayerControl::FStopAim()
{
	bIsAiming = false;

	SetAimingRotationMode(false);

	if (IsValid(PivotCamera))
	{
		// Ripristina esattamente la posizione originale della camera.
		PivotCamera->SocketOffset =DefaultCameraSocketOffset;
	}

	if (IsValid(CrossHairWidget))
	{
		CrossHairWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}




bool APlayerControl::EnsureCrosshairWidget()
{
	// Se esiste giˆ, non dobbiamo ricrearlo.
	if (IsValid(CrossHairWidget))
	{
		return true;
	}

	// Controlla che WBP_Crosshair sia stato assegnato.
	if (!CrossHairWidgetClass)
	{
		UE_LOG(LogTemp,Error,TEXT("EnsureCrosshairWidget: ""CrosshairWidgetClass non assegnata."));

		return false;
	}

	APlayerController* PlayerController =Cast<APlayerController>(GetController());

	// Nel BeginPlay il personaggio potrebbe non essere
	// ancora posseduto. Nel progetto single-player
	// proviamo a recuperare il primo PlayerController.
	if (!IsValid(PlayerController) && IsValid(GetWorld()))
	{
		PlayerController =GetWorld()->GetFirstPlayerController();
	}

	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTemp,Error,TEXT("EnsureCrosshairWidget: ""PlayerController non disponibile."));

		return false;
	}

	CrossHairWidget = CreateWidget<UUserWidget>(PlayerController,CrossHairWidgetClass);

	if (!IsValid(CrossHairWidget))
	{
		UE_LOG(LogTemp,Error,TEXT("EnsureCrosshairWidget: ""CreateWidget non riuscita."));

		return false;
	}

	CrossHairWidget->AddToViewport(10);

	CrossHairWidget->SetVisibility(ESlateVisibility::Hidden);

	CrosshairImageWidget=CrossHairWidget->GetWidgetFromName(TEXT("IIMG_Crosshair"));
	if (IsValid(CrosshairImageWidget))
	{
		CrosshairBaseRenderTranslation = CrosshairImageWidget->GetRenderTransform().Translation;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EnsureCrosshairWidget: ""Widget IIMG_cROSSHAIR NON TROVATO IN WBP_CrossHair."));
	}


	UE_LOG(LogTemp,Warning,TEXT("EnsureCrosshairWidget: ""WBP_Crosshair creato correttamente."));

	return true;
}


bool APlayerControl::CalculateAimTarget(FVector& OutTargetLocation,FHitResult& OutHitResult)
{
	OutTargetLocation = FVector::ZeroVector;
	OutHitResult = FHitResult();

	UWorld* World = GetWorld();
	APlayerController* PlayerController =Cast<APlayerController>(GetController());

	if (!IsValid(World) || !IsValid(PlayerController))
	{
		UE_LOG(LogTemp,Warning,TEXT("CalculateAimTarget: World o PlayerController non valido."));

		return false;
	}

	// Recupera le dimensioni reali del viewport.
	int32 ViewportWidth = 0;
	int32 ViewportHeight = 0;

	PlayerController->GetViewportSize(ViewportWidth,ViewportHeight);

	if (ViewportWidth <= 0 || ViewportHeight <= 0)
	{
		UE_LOG(LogTemp,Warning,TEXT("CalculateAimTarget: dimensioni viewport non valide."));

		return false;
	}

	// Centro esatto dello schermo, dove deve trovarsi il mirino.
	const float ScreenCenterX =static_cast<float>(ViewportWidth) * 0.5f;

	const float ScreenCenterY =static_cast<float>(ViewportHeight) * 0.5f;

	FVector CameraTraceStart = FVector::ZeroVector;
	FVector CameraTraceDirection = FVector::ZeroVector;

	// Trasforma il centro dello schermo in una posizione e
	// direzione tridimensionale nel mondo.
	const bool bDeprojected =PlayerController->DeprojectScreenPositionToWorld(ScreenCenterX,ScreenCenterY,CameraTraceStart,CameraTraceDirection);

	if (!bDeprojected)
	{
		UE_LOG(LogTemp,Warning,TEXT("CalculateAimTarget: DeprojectScreenPositionToWorld fallita."));

		return false;
	}

	CameraTraceDirection =
		CameraTraceDirection.GetSafeNormal();

	if (CameraTraceDirection.IsNearlyZero())
	{
		return false;
	}

	const FVector CameraTraceEnd =CameraTraceStart +CameraTraceDirection * AimTraceLength;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(PlayerAimTrace),true);

	// Il trace non deve colpire il personaggio o la sua pistola.
	QueryParams.AddIgnoredActor(this);

	if (IsValid(EquippedWeapon))
	{
		QueryParams.AddIgnoredActor(EquippedWeapon);
	}

	const bool bHit =World->LineTraceSingleByChannel(OutHitResult,CameraTraceStart,CameraTraceEnd,ECC_Visibility,QueryParams);

	// Se viene trovato qualcosa, la destinazione è il punto colpito.
	// Altrimenti spara nella direzione del mirino fino alla distanza massima.
	OutTargetLocation =bHit ? OutHitResult.ImpactPoint : CameraTraceEnd;

	// Anche quando il trace non colpisce niente, il calcolo è riuscito:
	// il proiettile deve poter essere sparato verso CameraTraceEnd.
	return true;
}

void APlayerControl::UpdateCrosshairFromWeapon()
{
	if (!IsValid(CrossHairWidget) || !IsValid(CrosshairImageWidget))
	{
		return;
	}

	APlayerController* PlayerController =Cast<APlayerController>(GetController());

	if (!IsValid(PlayerController))
	{
		return;
	}

	FVector AimTarget;
	FHitResult AimHit;

	CalculateAimTarget(AimTarget, AimHit);

	if (AimTarget.IsNearlyZero())
	{
		return;
	}

	FVector2D TargetWidgetPosition;

	const bool bProjected =UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PlayerController,AimTarget,TargetWidgetPosition,true);

	if (!bProjected)
	{
		return;
	}

	int32 ViewportWidth = 0;
	int32 ViewportHeight = 0;

	PlayerController->GetViewportSize(ViewportWidth,ViewportHeight);

	const float ViewportScale = FMath::Max(UWidgetLayoutLibrary::GetViewportScale(this),KINDA_SMALL_NUMBER);

	const FVector2D ViewportCenter(static_cast<float>(ViewportWidth) /(2.0f * ViewportScale),static_cast<float>(ViewportHeight) /(2.0f * ViewportScale));

	const FVector2D CrosshairOffset =TargetWidgetPosition - ViewportCenter;

	CrosshairImageWidget->SetRenderTranslation(CrosshairBaseRenderTranslation + CrosshairOffset);
}


void APlayerControl::FTestAimTrace()
{
	// Non  possibile provare il tiro in questi stati.
	if (bIsDead ||bIsStunned ||bIsPickingUp ||isCrouching ||_isInAir)
	{
		return;
	}

	// Il test funziona solamente quando pistola e mirino
	// sono effettivamente attivi.
	if (!isGunVisible ||!bIsAiming ||!IsValid(EquippedWeapon) ||!IsValid(CamPlayerOne))
	{
		return;
	}

	FVector AimTarget;
	FHitResult AimHit;

	const bool bHit =CalculateAimTarget(AimTarget, AimHit);

	const FVector CameraStart =CamPlayerOne->GetComponentLocation();//linea debug davanti a Cam

	// Verde quando  stato colpito qualcosa.
	// Rosso quando il controllo arriva alla distanza massima.
	const FColor CameraLineColor =bHit ? FColor::Green : FColor::Red;
	const FVector CameraDebugStart = CameraStart + CamPlayerOne->GetForwardVector() * 50;
	// Linea dalla telecamera al punto indicato dal mirino.
	//DrawDebugLine(GetWorld(),CameraStart,AimTarget,CameraLineColor,false,2.0f,0,2.0f);
	DrawDebugLine(GetWorld(), CameraStart, AimTarget, FColor::Cyan, false, 2.0f, 0, 0.5f);

	// Punto finale del controllo.
	DrawDebugSphere(GetWorld(),AimTarget,8.0f,12,CameraLineColor,false,2.0f);

	// Recupera la freccia MuzzlePoint di BP_Pistol.
	if (UArrowComponent* MuzzlePoint =EquippedWeapon->GetMuzzlePoint())
	{
		const FVector MuzzleLocation =MuzzlePoint->GetComponentLocation();

		// Linea gialla dalla canna al bersaglio.
		// Questa sarˆ la futura traiettoria del proiettile.
		DrawDebugLine(GetWorld(),MuzzleLocation,AimTarget,FColor::Yellow,false,2.0f,0,1.0f);//linea Canna/Bersaglio
	}

	const float DistanceInMeters =FVector::Distance(CameraStart, AimTarget) / 100.0f;

	if (bHit)
	{
		UE_LOG(LogTemp,Warning,TEXT("AimTrace: HIT a %.2f metri."),DistanceInMeters);
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("AimTrace: nessun impatto. Distanza %.2f metri."),DistanceInMeters);
	}
}


void APlayerControl::SetAimingRotationMode(bool bEnable)
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement) 
	{
		return;
	}
	if (bEnable)
	{
		Movement->bOrientRotationToMovement = false;
		Movement->bUseControllerDesiredRotation = true;
		Movement->RotationRate=FRotator(0.0f, 720.0f, 0.0f);
	}
	else
	{
		Movement->bUseControllerDesiredRotation = false;
		Movement->bOrientRotationToMovement = true;
		Movement->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	}
}

void APlayerControl::FFire()
{
	//const UCharacterMovementComponent* Movement = GetCharacterMovement();
	
	const bool bIsCurrentlyInAir = GetCharacterMovement() && GetCharacterMovement()->IsFalling();

	if (bIsDead || bIsStunned || bIsPickingUp || bIsRolling ||bIsCurrentlyInAir || !isGunVisible || !bIsAiming || bInventoryVisible || !IsValid(EquippedWeapon)) //no fire
	{
		return;
	}

	if (!AmmoComponent || !AmmoComponent->CanFire())
	{
		CreatePlayerStatsWidgetIfNeeded();
		if (PlayerStatsWidget)
		{
			PlayerStatsWidget->ShowNotificationMessage(FText::FromString(TEXT("Munizioni terminate. Trova le ricariche.")), EPlayerNotificationType::Warning, 2.5f);
		}
		return;
	}

	FVector TargetLocation;
	FHitResult AimHitResult;

	CalculateAimTarget(TargetLocation,AimHitResult);
	if (TargetLocation.IsNearlyZero())
	{
		return;
	}

	if (EquippedWeapon->FireAtTarget(TargetLocation))
	{
		AmmoComponent->ConsumeRound();
	}

	//EquippedWeapon->FireAtTarget(TargetLocation);
}

void APlayerControl::CreateAttachGrenadeHeld()
{
	// Se la granata esiste già non ne creiamo un'altra.
	if (IsValid(EquippedGrenadeHeld))
	{
		return;
	}

	if (!GrenadeHeldClass)
	{
		UE_LOG(LogTemp, Error,TEXT("GrenadeHeldClass non assegnata in BP_PlayerWoman."));
		return;
	}

	UWorld* World = GetWorld();
	USkeletalMeshComponent* CharacterMesh = GetMesh();

	if (!World || !CharacterMesh)
	{
		return;
	}

	if (GrenadeSocketName.IsNone() || !CharacterMesh->DoesSocketExist(GrenadeSocketName))
	{
		UE_LOG(LogTemp, Error,TEXT("GrenadeSocket %s non trovato."),*GrenadeSocketName.ToString());

		return;
	}

	FActorSpawnParameters SpawnParameters;

	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = this;

	SpawnParameters.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	EquippedGrenadeHeld =World->SpawnActor<AActor>(GrenadeHeldClass,FTransform::Identity,SpawnParameters);

	if (!IsValid(EquippedGrenadeHeld))
	{
		UE_LOG(LogTemp, Error,TEXT("Creazione BP_GrenadeHeld fallita."));
		return;
	}

	EquippedGrenadeHeld->AttachToComponent(CharacterMesh,FAttachmentTransformRules::SnapToTargetNotIncludingScale,GrenadeSocketName);

	EquippedGrenadeHeld->SetActorRelativeLocation(FVector::ZeroVector);
	EquippedGrenadeHeld->SetActorRelativeRotation(FRotator::ZeroRotator);
	EquippedGrenadeHeld->SetActorRelativeScale3D(FVector::OneVector);

	EquippedGrenadeHeld->SetActorEnableCollision(false);

	UE_LOG(LogTemp, Log,TEXT("Granata collegata al socket %s."),*GrenadeSocketName.ToString());
}
void APlayerControl::EquipGrenade()
{
	if (bIsDead ||bIsStunned ||bIsPickingUp ||bIsRolling)
	{
		return;
	}
	bRestoreWaponAfetrGranade = isGunVisible && IsValid(EquippedWeapon);

	// Spegne mira e mirino.
	FStopAim();

	// Nasconde la pistola.
	if (IsValid(EquippedWeapon))
	{
		EquippedWeapon->SetActorHiddenInGame(true);
	}

	isGunVisible = false;
	_GunLoadSoundPLayed = false;

	// Crea la granata e la collega alla mano.
	CreateAttachGrenadeHeld();

	if (!IsValid(EquippedGrenadeHeld))
	{
		return;
	}

	EquippedGrenadeHeld->SetActorHiddenInGame(false);

	bGrenadeEquipped = true;

	UpdateIWR();
	ApplyMovementSpeed();

	UE_LOG(LogTemp, Log,TEXT("Granata equipaggiata."));
}
void APlayerControl::FThrowGrenade()
{
	if (!bGrenadeEquipped ||bIsThrowingGrenade ||bIsDead ||bIsStunned ||bIsPickingUp ||bIsRolling)
	{
		return;
	}

	if (!GrenadeThrowMontage)
	{
		UE_LOG(LogTemp, Warning,TEXT("GrenadeThrowMontage non assegnato."));
		return;
	}

	bIsThrowingGrenade = true;

	const float MontageDuration =PlayAnimMontage(GrenadeThrowMontage);

	if (MontageDuration <= 0.0f)
	{
		bIsThrowingGrenade = false;
		return;
	}

	GetWorldTimerManager().ClearTimer(GrenadeThrowTimerHandle);
	GetWorldTimerManager().SetTimer(GrenadeThrowTimerHandle,this,&APlayerControl::EndGrenadeThrow,MontageDuration,false);
}

void APlayerControl::EndGrenadeThrow()
{
	bIsThrowingGrenade = false;
	if (bRestoreWaponAfetrGranade && IsValid(EquippedWeapon) && !bIsDead && !bIsStunned)
	{
		isGunVisible = true;
		EquippedWeapon->SetActorHiddenInGame(false);
		UpdateIWR();
		ApplyMovementSpeed();
		FStartAim();
	}
	bRestoreWaponAfetrGranade = false;
}

void APlayerControl::ShowDebugWarning(const FString& Message)
{
	if (PlayerStatsWidget)
	{
		PlayerStatsWidget->ShowNotificationMessage(FText::FromString(Message),EPlayerNotificationType::Warning,2.0f);
	}
}
void APlayerControl::TogglePauseMenu()
{
	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{
		ClosePauseMenu();
	}
	else
	{
		OpenPauseMenu();
	}
}


void APlayerControl::OpenPauseMenu()
{
	APlayerController* PlayerController =Cast<APlayerController>(GetController());

	if (!PlayerController || !PauseMenuWidgetClass)
	{
		return;
	}

	if (!PauseMenuWidget)
	{
		PauseMenuWidget =CreateWidget<UPauseMenuWidget>(PlayerController,PauseMenuWidgetClass);
	}

	if (!PauseMenuWidget)
	{
		return;
	}

	PauseMenuWidget->AddToViewport(100);

	UGameplayStatics::SetGamePaused(GetWorld(), true);

	FInputModeUIOnly InputMode;

	InputMode.SetWidgetToFocus(PauseMenuWidget->TakeWidget()
	);

	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;
}


void APlayerControl::ClosePauseMenu()
{
	APlayerController* PlayerController =Cast<APlayerController>(GetController());

	if (!PlayerController)
	{
		return;
	}

	UGameplayStatics::SetGamePaused(GetWorld(), false);

	if (PauseMenuWidget)
	{
		PauseMenuWidget->RemoveFromParent();
		PauseMenuWidget = nullptr;
	}

	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);

	PlayerController->bShowMouseCursor = false;
}


void APlayerControl::SetCurrentDoor(AKeyDoor* NewDoor) 
{
	CurrentDoor = NewDoor;
	if (PlayerStatsWidget && CurrentDoor)
	{
		PlayerStatsWidget->ShowInteractionPrompt(FText::FromString(TEXT("Porta Chiusa - Usa La chiave raccolta.")));
	}
}

void APlayerControl::ClearCurrentDoor(AKeyDoor* DoorToClaer)
{
	if (CurrentDoor == DoorToClaer)
	{
		CurrentDoor = nullptr;

		if (PlayerStatsWidget)
		{
			PlayerStatsWidget->HideInteractionPrompt();
		}
	}
}
void APlayerControl::SetCurrentHelicopter(AHelicopterToExit* NewHelicopter)
{
	CurrentHelicopter = NewHelicopter;

	if (PlayerStatsWidget)
	{
		if (bUSBCollected)
		{
			PlayerStatsWidget->ShowInteractionPrompt(FText::FromString(TEXT("Premi INTERAGISCI per lasciare l'isola")));
		}
		else
		{
			PlayerStatsWidget->ShowInteractionPrompt(FText::FromString(TEXT("Devi prima recuperare la USB")));
		}
	}
}


void APlayerControl::ClearCurrentHelicopter(AHelicopterToExit* HelicopterToClear)
{
	if (CurrentHelicopter == HelicopterToClear)
	{
		CurrentHelicopter = nullptr;

		if (PlayerStatsWidget)
		{
			PlayerStatsWidget->HideInteractionPrompt();
		}
	}
}

void APlayerControl::PrepareForExtracionCinematic()
{
	FStopAim();
	isGunVisible = false;
	if (IsValid(EquippedWeapon))
	{
		EquippedWeapon->SetActorHiddenInGame(true);
	}
	
	if (IsValid(CrossHairWidget))
	{
		CrossHairWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	UpdateIWR();
	HideGameplayHUD();
	SetActorHiddenInGame(true);//nasconde player durante il filmato;
	SetActorEnableCollision(false); //evita capsula player interferisca con Heli

	// silenzio enemy 
	TArray<AActor*>Enemies;  // creazione lista nemici
	UGameplayStatics::GetAllActorsOfClass(this, AEnemySoldier::StaticClass(), Enemies);  // ricerca attori classe UN della classe Aeoldier
	for (AActor* Actor : Enemies) // ciclo
	{
		if (AEnemySoldier* Enemy = Cast< AEnemySoldier>(Actor))  // array è actor dobbiamo cast perche volgio enemy
		{
			Enemy->SetFireSoundEnable(false);
		}
	}
}

void APlayerControl::HideGameplayHUD()
{
	bGmaeplayHUDVisible = false;
	if (PlayerStatsWidget)
	{
		PlayerStatsWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	if (CrossHairWidget)
	{
		CrossHairWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void APlayerControl::ShowGameplayHUD()
{
	bGmaeplayHUDVisible = true;
	if (PlayerStatsWidget)
	{
		PlayerStatsWidget->SetVisibility(ESlateVisibility::Visible);
	}
}




















