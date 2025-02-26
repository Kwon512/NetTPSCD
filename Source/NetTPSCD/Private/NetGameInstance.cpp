﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameInstance.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"

void UNetGameInstance::Init()
{
	Super::Init();

	if (auto subsystem = IOnlineSubsystem::Get())
	{
		sessionInterface = subsystem->GetSessionInterface();

		sessionInterface->OnCreateSessionCompleteDelegates.AddUObject( this , &UNetGameInstance::OnMyCreateRoomComplete );

		sessionInterface->OnFindSessionsCompleteDelegates.AddUObject( this , &UNetGameInstance::OnMyFindOtherRoomsComplete );

		sessionInterface->OnJoinSessionCompleteDelegates.AddUObject( this , &UNetGameInstance::OnMyJoinRoomComplete );

		sessionInterface->OnDestroySessionCompleteDelegates.AddUObject( this , &UNetGameInstance::OnMyExitRoomComplete );
	}
}

bool UNetGameInstance::IsInRoom()
{
	FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();

	return sessionInterface->IsPlayerInSession( FName(*myRoomName) , *netID );
}

void UNetGameInstance::CreateRoom( int32 maxPlayerCount , FString roomName )
{
	FOnlineSessionSettings setting;

	// 1. 데디케이트 서버인가??
	setting.bIsDedicated = false;
	// 2. 랜선인가?
	auto subSys = IOnlineSubsystem::Get();
	setting.bIsLANMatch = subSys->GetSubsystemName().IsEqual( "NULL" );
	// 3. 공개로 입장할 수 있는가? 아니면 친구초대로만???
	setting.bShouldAdvertise = true;
	// 4. 온라인 상태(presence)를 공개적으로 사용할것인가? -> ping정보
	setting.bUsesPresence = true;
	// 5. 중간입장이 가능한가?
	setting.bAllowJoinInProgress = true;
	setting.bAllowJoinViaPresence = true;
	// 6. 최대 입장 가능한 수 설정
	setting.NumPublicConnections = maxPlayerCount;
	// 7. 커스텀 정보 설정


	setting.Set( TEXT( "ROOM_NAME" ) , StringBase64Encode( roomName ) , EOnlineDataAdvertisementType::ViaOnlineServiceAndPing );
	setting.Set( TEXT( "HOST_NAME" ) , StringBase64Encode( myNickName ) , EOnlineDataAdvertisementType::ViaOnlineServiceAndPing );
	// 8. netID 찾기
	FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();

	UE_LOG( LogTemp , Warning , TEXT( "CreateRoom Start!!! roomNamd : %s, netID : %s" ) , *roomName , *netID->ToString() );

	sessionInterface->CreateSession( *netID , FName( *roomName ) , setting );
}

void UNetGameInstance::OnMyCreateRoomComplete( FName sessionName , bool bWasSuccessful )
{
	UE_LOG( LogTemp , Warning , TEXT( "OnMyCreateRoomComplete!!! sessionName : %s, bWasSuccessful : %d" ) , *sessionName.ToString() , bWasSuccessful );

	// 방을 생성했다면
	if (bWasSuccessful)
	{
		// 입장한 방의 이름을 기억하고싶다.
		myRoomName = sessionName.ToString();
		// 서버는 세계 여행을 떠나고싶다. 어디로???
		FString url = TEXT( "/Game/Net/Maps/BattleMap?listen" );
		GetWorld()->ServerTravel( url );
	}
}

void UNetGameInstance::FindOtherRooms()
{
	// 1. FOnlineSessionSearch객체를 생성
	roomSearch = MakeShareable( new FOnlineSessionSearch() );
	// 2. 세션 검색 조건 설정
	roomSearch->QuerySettings.Set( SEARCH_PRESENCE , true , EOnlineComparisonOp::Equals );
	// 3. 최대 검색 갯수를 정하고싶다.
	roomSearch->MaxSearchResults = 10;
	// 4. 랜선인지 아닌지를 정하고싶다.
	auto subSys = IOnlineSubsystem::Get();
	roomSearch->bIsLanQuery = subSys->GetSubsystemName().IsEqual( "NULL" );

	// 5. 검색을 하고싶다.
	sessionInterface->FindSessions( 0 , roomSearch.ToSharedRef() );

	if (onFindingRoomsDelegate.IsBound())
	{
		onFindingRoomsDelegate.Broadcast( true );
	}

}

void UNetGameInstance::OnMyFindOtherRoomsComplete( bool bWasSuccessful )
{
	if (onFindingRoomsDelegate.IsBound())
	{
		onFindingRoomsDelegate.Broadcast( false );
	}

	UE_LOG( LogTemp , Warning , TEXT( "OnMyFindOtherRoomsComplete : %d" ) , bWasSuccessful );

	for (int32 i = 0; i < roomSearch->SearchResults.Num(); i++)
	{
		auto r = roomSearch->SearchResults[i];

		if (false == r.IsValid())
			continue;

		FRoomInfo info;

		info.index = i;

		FString roomName_enc;
		FString hostName_enc;

		r.Session.SessionSettings.Get( TEXT( "ROOM_NAME" ) , roomName_enc );
		r.Session.SessionSettings.Get( TEXT( "HOST_NAME" ) , hostName_enc );

		info.roomName = StringBase64Decode( roomName_enc );
		info.hostName = StringBase64Decode( hostName_enc );

		int32 max = r.Session.SessionSettings.NumPublicConnections;
		// 현재 입장 플레이어 수 = 최대 - 입장가능 수
		int32 current = max - r.Session.NumOpenPublicConnections;
		info.playerCount = FString::Printf( TEXT( "%d / %d" ) , current , max );

		info.pingMS = FString::Printf( TEXT( "%dms" ) , r.PingInMs );

		info.PrintLog();

		// 만약 바인된 함수가 있다면
		if (onAddRoomInfoDelegate.IsBound())
		{
			onAddRoomInfoDelegate.Broadcast( info );
		}
	}
}

void UNetGameInstance::JoinRoom( int32 index )
{
	auto r = roomSearch->SearchResults[index];
	FString sessionName;
	r.Session.SessionSettings.Get( TEXT( "ROOM_NAME" ) , sessionName );
	sessionName = StringBase64Decode( sessionName );
	sessionInterface->JoinSession( 0 , FName( *sessionName ) , r );
}

void UNetGameInstance::OnMyJoinRoomComplete( FName sessionName , EOnJoinSessionCompleteResult::Type result )
{
	// 성공했다면?
	if (EOnJoinSessionCompleteResult::Success == result)
	{
		// 입장한 방의 이름을 기억하고싶다.
		myRoomName = sessionName.ToString();

		// 서버의 주소를 받아와서
		FString url;
		sessionInterface->GetResolvedConnectString( sessionName , url );
		// 여행을 떠나고 싶다.
		auto pc = GetWorld()->GetFirstPlayerController();
		pc->ClientTravel( url , TRAVEL_Absolute );
	}
	// 그렇지않다면
	else
	{
		UE_LOG( LogTemp , Warning , TEXT( "Join Session Failed... : %d" ), result );
	}
}

void UNetGameInstance::ExitRoom()
{
	ServerExitRoom();
}

void UNetGameInstance::ServerExitRoom_Implementation()
{
	MultiExitRoom();
}

void UNetGameInstance::MultiExitRoom_Implementation()
{
	sessionInterface->DestroySession( FName( *myRoomName ) );
}

void UNetGameInstance::OnMyExitRoomComplete(FName sessionName, bool bWasSuccessful)
{
	// 플레이어는 LobbyMap으로 여행을 떠나고싶다.
	auto pc = GetWorld()->GetFirstPlayerController();
	FString url = TEXT( "/Game/Net/Maps/LobbyMap" );
	pc->ClientTravel( url , TRAVEL_Absolute );
}


FString UNetGameInstance::StringBase64Encode( const FString& str )
{
	// Set 할 때 : FString -> UTF8 (std::string) -> TArray<uint8> -> base64 로 Encode
	std::string utf8String = TCHAR_TO_UTF8( *str );
	TArray<uint8> arrayData = TArray<uint8>( (uint8*)(utf8String.c_str()) , utf8String.length() );
	return FBase64::Encode( arrayData );
}

FString UNetGameInstance::StringBase64Decode( const FString& str )
{
	// Get 할 때 : base64 로 Decode -> TArray<uint8> -> TCHAR
	TArray<uint8> arrayData;
	FBase64::Decode( str , arrayData );
	std::string ut8String( (char*)(arrayData.GetData()) , arrayData.Num() );
	return UTF8_TO_TCHAR( ut8String.c_str() );
}

