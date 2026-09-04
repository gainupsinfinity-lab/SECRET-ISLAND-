// Fill out your copyright notice in the Description page of Project Settings.



#include "AnimIstancePlayerOne.h"
#include "KismetAnimationLibrary.h"
#include "PlayerControl.h"


void UAnimIstancePlayerOne ::NativeUpdateAnimation(float DeltaSeconds) 
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	Actor = GetOwningActor();
	if (Actor != nullptr)
	{
		PlayerControl = Cast<APlayerControl>(Actor);
		if (PlayerControl)
		{
			Speed = PlayerControl->_Speed;
			const FVector Velocity = PlayerControl->GetVelocity();
			Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, PlayerControl->GetActorRotation());
			IWR = PlayerControl->_IWR;
			isInAir = PlayerControl->_isInAir;
			VerticalVelocity = PlayerControl->_VerticalVelocity;
			isJumping = PlayerControl->_isJumping;
			isForwardJump = PlayerControl->_isForwardJump;
            bIsAiming = PlayerControl->IsAiming();
			if (bIsAiming)
			{
				const FRotator AimDelta =(PlayerControl->GetBaseAimRotation()-PlayerControl->GetActorRotation()).GetNormalized();
				AimPitch = FMath::Clamp(AimDelta.Pitch,-65.0f,55.0f);
				AimYaw = FMath::Clamp(AimDelta.Yaw,-90.0f, 90.0f);}
			else
			{
				AimPitch = 0.0f;
				AimYaw = 0.0f;
			}
		}
	}
}