// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerController.h"

#include "BattleGameMode.h"
#include "GameFramework/Character.h"

void ANetPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// 태어날 때 내가 서버라면 게임모드를 기억하고싶다.
	if (HasAuthority())
	{
		gm = Cast<ABattleGameMode>(GetWorld()->GetAuthGameMode());
	}
}

// 이곳은 서버에서 호출되는 함수이다.
void ANetPlayerController::ServerRetry_Implementation()
{
	// 현재 캐릭터를 기억하고
	auto character = this->GetCharacter();
	// UnPossess하고
	UnPossess();
	// 기억한 캐릭터를 파괴하고
	character->Destroy();

	// 게임모드에게 재시작 하라고 하고싶다.
	check( gm );

	if (gm)
	{
		gm->RestartPlayer( this );
	}
}
