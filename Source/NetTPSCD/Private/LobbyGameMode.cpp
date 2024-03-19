// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

ALobbyGameMode::ALobbyGameMode()
{
	//bUseSeamlessTravel = true;
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor( true );
}

