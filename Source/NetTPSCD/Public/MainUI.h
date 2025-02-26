﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUI.generated.h"

/**
 * 
 */
UCLASS()
class NETTPSCD_API UMainUI : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;


	// ImageCrosshair를 BindWidget 해보세요.
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	class UImage* ImageCrosshair;

	void SetActiveCrosshair(bool bActive);

	// 태어날 때 플레이어가 가진 총알의 최대 갯수만큼 표현하고싶다.
	// 총을 쏘면 하나씩 제거하고싶다.
	// 재장전하면 다시 다 채우고싶다.
	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UUniformGridPanel* grid_bullet;

	UPROPERTY( EditDefaultsOnly )
	TSubclassOf<class UUserWidget> bulletUIFactory;

	void ReloadBulletUI(int32 maxBulletCount);
	void AddBulletUI();
	void RemoveBulletUI(int32 index);

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UProgressBar* bar_hp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float hp = 1.0f;

	UPROPERTY(EditDefaultsOnly, meta=(BindWidgetAnim), Transient)
	class UWidgetAnimation* HitAnimation;

	void PlayHitAnim();

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UCanvasPanel* gameOverUI;

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UButton* btn_retry;

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UButton* btn_quit;

	void SetShowGameOverUI( bool bShow );

	UFUNCTION()
	void OnMyClickRetry();

	UFUNCTION()
	void OnMyClickQuit();

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UButton* btn_exit;
	

	

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UTextBlock* txt_players;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UScrollBox* scroll_msg;

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UEditableText* edit_sendMsg;

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UButton* btn_sendMsg;

	// 메시지UI의 공장을 가지고싶다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UMessageUI> msgUIFactory;

	// btn_sendMsg를 누르면 호출할 함수
	UFUNCTION()
	void OnMySendMsg();

	// 서버통신 후 응답처리할 함수
	void RecvMsg(const FString& msg );
};
