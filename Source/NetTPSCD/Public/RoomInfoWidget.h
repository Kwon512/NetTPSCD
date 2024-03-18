// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoomInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class NETTPSCD_API URoomInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UTextBlock* txt_roomName;
	
	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UTextBlock* txt_hostName;

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UTextBlock* txt_playerCount;

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UTextBlock* txt_ping;

	int32 roomIndex;

	void SetInfo( const struct FRoomInfo& info );

	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UButton* btn_join;

	UFUNCTION()
	void OnMyJoinRoom();
};
