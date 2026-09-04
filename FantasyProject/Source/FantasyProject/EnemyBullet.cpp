// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBullet.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"


AEnemyBullet::AEnemyBullet():
	Collision(nullptr),
	BulletMesh(nullptr),
	ProjectileMovement(nullptr),
	Damage(10.0f),
	Speed(2000.0f)
{
 	
	PrimaryActorTick.bCanEverTick = false;
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);

	Collision->InitSphereRadius(4.0f);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Collision->SetCollisionObjectType(ECC_WorldDynamic);
	Collision->SetCollisionResponseToAllChannels(ECR_Block);
	Collision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	Collision->SetNotifyRigidBodyCollision(true);

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	BulletMesh->SetupAttachment(Collision);
	BulletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BulletMesh->SetRelativeScale3D(FVector(5.0f, 5.0f, 5.0f));

	ProjectileMovement =CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	ProjectileMovement->UpdatedComponent = Collision;
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	InitialLifeSpan = 3.0f;

}

// Chiamat per Spawn
void AEnemyBullet::BeginPlay()
{
	Super::BeginPlay();
	if (AActor* OwnerActor = GetOwner())
	{
		Collision->IgnoreActorWhenMoving(OwnerActor, true);
		Collision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	
	}

	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
	ProjectileMovement->Velocity = GetActorForwardVector() * Speed;

	Collision->OnComponentHit.AddDynamic(this,&AEnemyBullet::HandleHit);
	
}
void AEnemyBullet::SetDamage(float InDamage)
{
	Damage = FMath::Max(0.0f, InDamage);
}

void AEnemyBullet::HandleHit(UPrimitiveComponent* HitComponent,AActor* OtherActor,UPrimitiveComponent* OtherComponent,FVector NormalImpulse,const FHitResult& Hit)
{
	if (!IsValid(OtherActor) ||OtherActor == this ||OtherActor == GetOwner())
	{
		return;
	}

	UGameplayStatics::ApplyPointDamage(OtherActor,Damage,GetActorForwardVector(),Hit,GetInstigatorController(),this,UDamageType::StaticClass());

	Destroy();
}
