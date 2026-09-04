// Fill out your copyright notice in the Description page of Project Settings.


#include "Landmine.h"
#include "PlayerControl.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ALandmine::ALandmine():
    OuterDamageRadius(20.0f),
    LethalRadius(7.0f),
    TriggerPadding(30.0f),
    MaxFootHeightDifference(25.0f),
    DamagePercent(50.0f),
    InitialEvaluationDelay(0.0f),
    EvaluationInterval(0.02f),
    DestroyDelay(3.0f),
    ExplosionEffect(nullptr),
    SandExplosionEffect(nullptr),
    ExplosionSound(nullptr),
    bExploded(false)

{
 	
	PrimaryActorTick.bCanEverTick = false; 
    SceneRoot =CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));

    RootComponent = SceneRoot; // seem actor root uff.sceneRoot

    MineMesh =CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MineMesh")); // create mesh mine

    MineMesh->SetupAttachment(SceneRoot); //herarchy root

    // La mesh  solamente grafica.
    // La rilevazione viene fatta da TriggerSphere.
    MineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); //mesh  mine no collision 

    MineMesh->SetGenerateOverlapEvents(false); // no rrivela on

    TriggerSphere =CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere")); // crea sfera invisibile

    TriggerSphere->SetupAttachment(SceneRoot);  //hierarchi

    TriggerSphere->InitSphereRadius(OuterDamageRadius + TriggerPadding); //raddio finale

    TriggerSphere->SetRelativeLocation(FVector(0.0f, 0.0f, 3.0f)); //alzata 3 unita'

    TriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly); //abilita collisione solo con obj

    TriggerSphere->SetCollisionObjectType(ECC_WorldDynamic);// obj dynamic 

    TriggerSphere->SetCollisionResponseToAllChannels(ECR_Ignore);//no action 

    TriggerSphere->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap); //only Pan overlap

    TriggerSphere->SetGenerateOverlapEvents(true); //event overlap uqnado entra sfera

}


void ALandmine::BeginPlay()
{
	Super::BeginPlay();
    TriggerSphere->SetSphereRadius(OuterDamageRadius + TriggerPadding,true);

    TriggerSphere->OnComponentBeginOverlap.AddDynamic(this,&ALandmine::OnTriggerBeginOverlap);

    TriggerSphere->OnComponentEndOverlap.AddDynamic(this,&ALandmine::OnTriggerEndOverlap);

	
}
void ALandmine::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,
    bool bFromSweep,const FHitResult& SweepResult)
{
    if (bExploded|| !IsValid(OtherActor) || !IsValid(OtherComp) )
    {
        return;
    }

    // La mina deve reagire esclusivamente ai sensori dei piedi.
    if (!OtherComp->ComponentHasTag(TEXT("PlayerFoot")))
    {
        return;
    }

    APlayerControl* Player =Cast<APlayerControl>(OtherActor);

    if (!IsValid(Player) || Player->IsDead())
    {
        return;
    }

    PendingPlayer = Player;
    PendingFootComponent = OtherComp;

    if (!GetWorldTimerManager().IsTimerActive(EvaluationTimerHandle))
    {
        GetWorldTimerManager().SetTimer(EvaluationTimerHandle,this,&ALandmine::EvaluatePendingFoot,EvaluationInterval,true,InitialEvaluationDelay);
    }
}

void ALandmine::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex)
{
    if (PendingFootComponent.IsValid()&&PendingFootComponent.Get() == OtherComp)
    {
        ClearPendingFoot();
    }
}

void ALandmine::EvaluatePendingFoot()
{
    if (bExploded||!PendingPlayer.IsValid()|| !PendingFootComponent.IsValid())
    {
        ClearPendingFoot();
        return;
    }

    APlayerControl* Player = PendingPlayer.Get();

    UPrimitiveComponent* FootComponent =PendingFootComponent.Get();

    if (!IsValid(Player) || Player->IsDead())
    {
        ClearPendingFoot();
        return;
    }

    if (!TriggerSphere->IsOverlappingComponent(FootComponent))
    {
        ClearPendingFoot();
        return;
    }

    const FVector MineLocation =TriggerSphere->GetComponentLocation();

    const FVector FootLocation =FootComponent->GetComponentLocation();

    const float HeightDifference =FMath::Abs(FootLocation.Z - MineLocation.Z);

    // Il piede deve trovarsi realmente vicino al terreno
    // su cui  posizionata la mina.
    if (HeightDifference > MaxFootHeightDifference)
    {
        return;
    }

    const float DistanceFromCenter =FVector::Dist2D(FootLocation,MineLocation);

    if (DistanceFromCenter > OuterDamageRadius)
    {
        return;
    }

    const bool bLethalHit =DistanceFromCenter <= LethalRadius;

    Explode(Player,bLethalHit);
}

void ALandmine::Explode(APlayerControl* Player,bool bLethalHit)
{
    if (bExploded)
    {
        return;
    }

    bExploded = true;

    ClearPendingFoot();

    TriggerSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (ExplosionEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),ExplosionEffect,GetActorLocation(),GetActorRotation());
    }
    if (SandExplosionEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SandExplosionEffect, GetActorLocation(), GetActorRotation());
    }

    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this,ExplosionSound,GetActorLocation());
    }

    if (IsValid(Player))
    {
        Player->ApplyLandmineExplosion(bLethalHit,DamagePercent);
    }

    MineMesh->SetVisibility(false, true);

    SetLifeSpan(DestroyDelay);
}

void ALandmine::ClearPendingFoot()
{
    GetWorldTimerManager().ClearTimer(EvaluationTimerHandle);

    PendingPlayer.Reset();
    PendingFootComponent.Reset();
}





