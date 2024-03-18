// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NetGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FRoomInfo
{
	GENERATED_BODY()

	UPROPERTY( EditDefaultsOnly )
	int32 index;
	UPROPERTY( EditDefaultsOnly )
	FString roomName;
	UPROPERTY( EditDefaultsOnly )
	FString hostName;
	UPROPERTY( EditDefaultsOnly )
	FString playerCount;
	UPROPERTY( EditDefaultsOnly )
	FString pingMS;

	FORCEINLINE void PrintLog() const
	{
		UE_LOG( LogTemp , Warning , TEXT( "RoomName : %s, HostName:%s, PlayerCount : %s, Ping : %s" ) , *roomName , *hostName , *playerCount , *pingMS );
	}
};

// 방을 찾았을 때 UI를 만들고싶은데 UI를 만드는 일은 LobbyWidget에서 한다.
// 그래서 델리게이트를 만들어서 LobbyWidget에서 AddRoomInfoWidget을 델리게이트에 추가 하고싶다.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FAddRoomInfoDelegate , const FRoomInfo& , roomInfo );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FFindingRoomsDelegate , bool , bActive );


UCLASS()
class NETTPSCD_API UNetGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// 시작할 때 세션인터페이스를 기억하고싶다.
	virtual void Init() override;

	IOnlineSessionPtr sessionInterface;

	// 방검색 후 방생성에 관련 델리게이트
	FAddRoomInfoDelegate onAddRoomInfoDelegate;
	// 방검색에 타이밍에 관련 델리게이트
	FFindingRoomsDelegate onFindingRoomsDelegate;

	// 세션(session) == 방(room)
	// 세션생성요청함수
	FString hostName = TEXT("Jacob");
	void CreateRoom(int32 maxPlayerCount, FString roomName);
	// 세션생성응답함수
	void OnMyCreateRoomComplete( FName sessionName , bool bWasSuccessful );

	TSharedPtr<FOnlineSessionSearch> roomSearch;
	// 세션검색요청
	void FindOtherRooms();
	// 세션검색응답
	void OnMyFindOtherRoomsComplete( bool bWasSuccessful );

	// 방 입장 요청
	void JoinRoom(int32 index);
	// 방 입장 응답
	void OnMyJoinRoomComplete( FName sessionName , EOnJoinSessionCompleteResult::Type result );
	
};
