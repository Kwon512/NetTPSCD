// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerController.h"

#include "BattleGameMode.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpectatorPawn.h"

void ANetPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// 태어날 때 내가 서버라면 게임모드를 기억하고싶다.
	if (HasAuthority())
	{
		gm = Cast<ABattleGameMode>(GetWorld()->GetAuthGameMode());
	}
}

void ANetPlayerController::ServerRetrySpectator_Implementation()
{
	// 재시작요청을 하면 서버RPC로 관전자를 생성해서 빙의하라고한다.
	// 관전자를 생성하고
	auto player = GetCharacter();
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	auto spectator = GetWorld()->SpawnActor<ASpectatorPawn>( gm->SpectatorClass , player->GetActorLocation(), player->GetActorRotation(), params );
	// 관전자로 빙의
	Possess( spectator );
	player->Destroy();
	// 5초후에 다시 원래 플레이어로 시작하도록 처리하고싶다.
	FTimerHandle handle;
	GetWorldTimerManager().SetTimer( handle , [&]()
	{
		ServerRetry();
	} , 5 , false );
}

// 이곳은 서버에서 호출되는 함수이다.
void ANetPlayerController::ServerRetry_Implementation()
{
	// 현재 캐릭터를 기억하고
	auto character = this->GetPawn();
	// UnPossess하고
	UnPossess();
	// 기억한 캐릭터를 파괴하고
	character->Destroy();

	// 게임모드에게 재시작 하라고 하고싶다.
	if (nullptr == gm)
	{
		gm = Cast<ABattleGameMode>( GetWorld()->GetAuthGameMode() );
	}

	if (gm)
	{
		gm->RestartPlayer( this );
	}
}
