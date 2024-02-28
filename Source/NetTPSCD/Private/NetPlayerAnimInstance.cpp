// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerAnimInstance.h"

#include "NetTPSCDCharacter.h"

void UNetPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	// player를 기억하고싶다.
	player = Cast<ANetTPSCDCharacter>(GetOwningActor());
}

void UNetPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (nullptr == player)
		return;

	// speed, direction값을 채우고싶다.
	const FVector velocity = player->GetVelocity();

	speed = FVector::DotProduct(velocity, player->GetActorForwardVector());

	direction = FVector::DotProduct(velocity, player->GetActorRightVector());

	bHasPistol = player->bHasPistol;
}
