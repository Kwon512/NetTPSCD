// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerAnimInstance.h"

#include "NetTPSCDCharacter.h"

void UNetPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	// player를 기억하고싶다.
	player = Cast<ANetTPSCDCharacter>( GetOwningActor() );
}

void UNetPlayerAnimInstance::NativeUpdateAnimation( float DeltaSeconds )
{
	Super::NativeUpdateAnimation( DeltaSeconds );

	if (nullptr == player)
		return;

	// speed, direction값을 채우고싶다.
	const FVector velocity = player->GetVelocity();

	speed = FVector::DotProduct( velocity , player->GetActorForwardVector() );

	direction = FVector::DotProduct( velocity , player->GetActorRightVector() );

	bHasPistol = player->bHasPistol;

	// Player의 Pitch값을 가져와서 PitchAngle에 대입하고싶다.
	pitchAngle = -player->GetBaseAimRotation().GetNormalized().Pitch;

	// pitchAngle값을 -60 ~ 60안에 가두고싶다.
	pitchAngle = FMath::Clamp( pitchAngle , -60 , 60 );

	// 플레이어의 bDie를 기억하고싶다.
	bDie = player->bDie;


	//FVector	forwardVector = player->GetActorForwardVector();
	//FVector direction = FVector( 1 , 1 , 0 );

	//float dot = FVector::DotProduct( forwardVector , direction );
	//float angle =  FMath::RadiansToDegrees( FMath::Acos( dot ) );
	//// -180 ~ 180
	//if (angle < 0)
	//	angle = 360 + angle;
}

void UNetPlayerAnimInstance::PlayFireAnimation()
{
	// 총을 잡고 있을때만 총쏘기 애니메이션을 하고싶다.
	if (bHasPistol && fireMontage)
	{
		Montage_Play( fireMontage );
	}
}

void UNetPlayerAnimInstance::PlayReloadAnimation()
{
	if (bHasPistol && reloadMontage)
	{
		Montage_Play( reloadMontage );
	}
}

void UNetPlayerAnimInstance::AnimNotify_OnReloadFinished()
{
	// 캐릭터에게 실제로 총알을 채워라 라고 하고싶다.
	if (nullptr == player)
		return;

	// 서버에 InitAmmo를 해달라고 요청해야한다.
	player->ServerInitAmmo();
}

void UNetPlayerAnimInstance::AnimNotify_DieEnd()
{
	// 플레이어의 DamageProcess를 호출하고싶다.
	if (player)
	{
		player->DamageProcess();
	}
}
