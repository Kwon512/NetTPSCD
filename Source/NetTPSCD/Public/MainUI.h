// Fill out your copyright notice in the Description page of Project Settings.

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


};
