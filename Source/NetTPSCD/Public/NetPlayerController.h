// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NetPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NETTPSCD_API ANetPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	// 태어날 때 내가 서버라면 게임모드를 기억하고싶다.
	UPROPERTY()
	class ABattleGameMode* gm;

	// MainUI를 화면에 보이게하고 싶다.
// BeginPlay에서 MainUI를 생성해서 기억하고 AddToViewport하고싶다.
	UPROPERTY( EditDefaultsOnly )
	TSubclassOf<class UMainUI> mainUIFactory;

	UPROPERTY()
	class UMainUI* mainUI;


	// 재시작요청이 오면 서버RPC로 서버에게 현재 플레이어를 UnPossess하고 파괴하고, 게임모드에게 재시작 하라고 하고싶다.
	UFUNCTION(Server, Reliable)
	void ServerRetry();

	// 재시작요청을 하면 서버RPC로 관전자를 생성해서 빙의하라고한다.
	// 5초후에 다시 원래 플레이어로 시작하도록 처리하고싶다.
	UFUNCTION( Server , Reliable )
	void ServerRetrySpectator();

	
};
