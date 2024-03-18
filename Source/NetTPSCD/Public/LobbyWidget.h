// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class NETTPSCD_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UPROPERTY()
	class UNetGameInstance* gi;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UEditableText* edit_roomName;

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class USlider* slider_maxPlayer;

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UTextBlock* text_maxPlayer;

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UButton* btn_doCreateRoom;


	// slider_maxPlayer를 이동시키면 text_maxPlayer의 값도 바뀌게 하고싶다.
	UFUNCTION()
	void OnMyValueChage_maxPlayer( float value );

	// 버튼을 누르면 UNetGameInstance::CreateRoom 를 호출하고싶다.
	UFUNCTION()
	void OnMyClicked_doCreateRoom();

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UWidgetSwitcher* widgetSwitcherLobby;

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UButton* btn_goCreateRoom;

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UButton* btn_goFindRoom;

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UButton* btn_goMenuFromCreateRoom;

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UButton* btn_goMenuFromFindRoom;

	void SwitchPanel( int32 index );

	UFUNCTION()
	void OnMyGoMenu();

	UFUNCTION()
	void OnMyGoCreateRoom();

	UFUNCTION()
	void OnMyGoFindRoom();

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UScrollBox* scroll_roomList;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class URoomInfoWidget> roomInfoFactory;

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UButton* btn_doFindRoomList;

	UFUNCTION()
	void OnMyDoFindRoomList();

	UFUNCTION()
	void AddRoomInfoWidget(const struct FRoomInfo& roomInfo);

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UTextBlock* txt_findingRooms;

	UFUNCTION()
	void SetFindActive( bool bActive );
	
};
