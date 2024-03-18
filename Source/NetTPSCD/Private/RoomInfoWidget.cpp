// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomInfoWidget.h"

#include "Components/TextBlock.h"

void URoomInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void URoomInfoWidget::SetInfo( int32 idx , FString roomName , FString _hostName , FString playerCount , FString pingMS )
{
	roomIndex = idx;

	txt_roomName->SetText( FText::FromString( *roomName ) );
	txt_hostName->SetText( FText::FromString( *_hostName ) );
	txt_playerCount->SetText( FText::FromString( *playerCount ) );
	txt_ping->SetText( FText::FromString( *pingMS ) );
}
