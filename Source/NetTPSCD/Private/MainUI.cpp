// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUI.h"
#include "Components/Image.h"

void UMainUI::SetActiveCrosshair( bool bActive )
{
	// 만약 크로스헤어 이미지를 bActive가 true라면 보이고 false면 안보이게 하고싶다.
	// 플레이어가 태어날때 안보이게
	// 총 집으면 보이게
	// 총 놓으면 안보이게
	if (bActive)
	{
		ImageCrosshair->SetVisibility( ESlateVisibility::Visible );
	}
	else
	{
		ImageCrosshair->SetVisibility( ESlateVisibility::Hidden );
	}
}
