// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"

#include "NetGameInstance.h"
#include "RoomInfoWidget.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

//#define SWITCHER_INDEX_MENU 0
//#define SWITCHER_INDEX_CREATEROOM 1
//#define SWITCHER_INDEX_FINDROOM 2

const int SWITCHER_INDEX_MENU = 0;
const int SWITCHER_INDEX_CREATEROOM = 1;
const int SWITCHER_INDEX_FINDROOM = 2;

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	gi = GetWorld()->GetGameInstance<UNetGameInstance>();
	if (gi)
	{
		gi->onAddRoomInfoDelegate.AddDynamic( this , &ULobbyWidget::AddRoomInfoWidget );
		gi->onFindingRoomsDelegate.AddDynamic( this , &ULobbyWidget::SetFindActive );
	}

	btn_doCreateRoom->OnClicked.AddDynamic( this , &ULobbyWidget::OnMyClicked_doCreateRoom );
	btn_doFindRoomList->OnClicked.AddDynamic( this , &ULobbyWidget::OnMyDoFindRoomList );

	btn_goCreateRoom->OnClicked.AddDynamic( this , &ULobbyWidget::OnMyGoCreateRoom );
	btn_goFindRoom->OnClicked.AddDynamic( this , &ULobbyWidget::OnMyGoFindRoom );
	btn_goMenuFromCreateRoom->OnClicked.AddDynamic( this , &ULobbyWidget::OnMyGoMenu );
	btn_goMenuFromFindRoom->OnClicked.AddDynamic( this , &ULobbyWidget::OnMyGoMenu );


	slider_maxPlayer->OnValueChanged.AddDynamic( this , &ULobbyWidget::OnMyValueChage_maxPlayer );

	text_maxPlayer->SetText( FText::AsNumber( slider_maxPlayer->GetValue() ) );
	slider_maxPlayer->SetMinValue( 2 );
	slider_maxPlayer->SetMaxValue( 10 );
	slider_maxPlayer->SetStepSize( 1 );
	slider_maxPlayer->MouseUsesStep = true;
}

void ULobbyWidget::OnMyValueChage_maxPlayer( float value )
{
	text_maxPlayer->SetText( FText::AsNumber( value ) );
}

void ULobbyWidget::OnMyClicked_doCreateRoom()
{
	// 만약 방이름이 비어있으면 함수를 종료하고싶다.
	FString roomName = edit_roomName->GetText().ToString();

	if (roomName.TrimStartAndEnd().IsEmpty())
	{
		UE_LOG( LogTemp , Warning , TEXT( "방 이름을 안 적었어요!!" ) );
		return;
	}

	TArray<FString> badWords;
	badWords.Add( TEXT( "바보" ) );
	badWords.Add( TEXT( "똥개" ) );
	for (int i = 0; i < badWords.Num(); i++)
	{
		if (roomName.Contains( badWords[i] ))
		{
			UE_LOG( LogTemp , Warning , TEXT( "방 이름이 적절하지 않습니다." ) );
			return;
		}
	}


	// UNetGameInstance::CreateRoom 를 호출하고싶다.
	int32 maxPlayer = slider_maxPlayer->GetValue();
	gi->CreateRoom( maxPlayer , roomName );
}

void ULobbyWidget::SwitchPanel( int32 index )
{
	widgetSwitcherLobby->SetActiveWidgetIndex( index );
}

void ULobbyWidget::OnMyGoMenu()
{
	SwitchPanel( SWITCHER_INDEX_MENU );
}

void ULobbyWidget::OnMyGoCreateRoom()
{
	SwitchPanel( SWITCHER_INDEX_CREATEROOM );
}

void ULobbyWidget::OnMyGoFindRoom()
{
	SwitchPanel( SWITCHER_INDEX_FINDROOM );
	// 메뉴에서 방찾기로 진입시에 조회를 하고싶다.
	OnMyDoFindRoomList();
}

void ULobbyWidget::OnMyDoFindRoomList()
{
	scroll_roomList->ClearChildren();
	if (gi)
	{
		gi->FindOtherRooms();
	}
}

void ULobbyWidget::AddRoomInfoWidget( const FRoomInfo& info )
{
	if (nullptr == scroll_roomList || nullptr == roomInfoFactory)
		return;

	// 위젯을 생성해서 roomInfoFactory
	auto ui = CreateWidget<URoomInfoWidget>( GetWorld() , roomInfoFactory );
	ui->SetInfo( info );
	// scroll_roomList의 자식으로 붙이고싶다.
	scroll_roomList->AddChild( ui );
}

void ULobbyWidget::SetFindActive( bool bActive )
{
	// bActive가 true라면
	if (bActive)
	{
		// btn_doFindRoomList 버튼을 비활성화 하고싶다.
		btn_doFindRoomList->SetIsEnabled( false );
		// txt_findingRooms를 보이게 하고싶다.
		txt_findingRooms->SetVisibility( ESlateVisibility::Visible );
	}
	// 그렇지 않다면
	else {
		// btn_doFindRoomList 버튼을 활성화 하고싶다.
		btn_doFindRoomList->SetIsEnabled( true );
		// txt_findingRooms를 안 보이게 하고싶다.
		txt_findingRooms->SetVisibility( ESlateVisibility::Hidden );
	}
}
