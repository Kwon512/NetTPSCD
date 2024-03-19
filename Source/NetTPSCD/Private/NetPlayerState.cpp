// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerState.h"

#include "NetGameInstance.h"

void ANetPlayerState::BeginPlay()
{
	Super::BeginPlay();

	// 내가 로컬플레이어라면
	auto pc = GetPlayerController();
	if (pc && pc->IsLocalController())
	{
		// UNetGameInstance의 nickname을 서버로 보내서
		// 서버에서 nickname을 내 이름으로 하고싶다.
		auto gi = GetGameInstance<UNetGameInstance>();
		ServerSetNickname( gi->myNickName );
	}
}

void ANetPlayerState::ServerSetNickname_Implementation(const FString& newNickmane)
{
	// 서버RPC함수에서 	
	SetPlayerName( newNickmane );
}

