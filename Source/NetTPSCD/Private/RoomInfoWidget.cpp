// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomInfoWidget.h"

#include "NetGameInstance.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void URoomInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	btn_join->OnClicked.AddDynamic( this , &URoomInfoWidget::OnMyJoinRoom );
}

void URoomInfoWidget::SetInfo( const FRoomInfo& info )
{
	roomIndex = info.index;

	txt_roomName->SetText( FText::FromString( *info.roomName ) );
	txt_hostName->SetText( FText::FromString( *info.hostName ) );
	txt_playerCount->SetText( FText::FromString( *info.playerCount ) );
	txt_ping->SetText( FText::FromString( *info.pingMS ) );
}

void URoomInfoWidget::OnMyJoinRoom()
{
	auto gi = Cast<UNetGameInstance>( GetWorld()->GetGameInstance() );
	gi->JoinRoom( roomIndex );
}
