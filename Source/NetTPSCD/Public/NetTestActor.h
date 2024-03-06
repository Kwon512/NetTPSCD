// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetTestActor.generated.h"

UCLASS()
class NETTPSCD_API ANetTestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANetTestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* meshComp;

	void PrintNetLog();

	UPROPERTY(EditDefaultsOnly)
	float detectRadius = 300.0f;

	void FindOwner();


	// 변수 리플리케이트
	// 함수 리플리케이트 : (RPC, Remote Procedure Call)

	// 속성 리플리케이트 : 이벤트방식
	UPROPERTY(ReplicatedUsing=OnRep_RotYaw)
	float rotYaw;

	UFUNCTION()
	void OnRep_RotYaw();

	// 보간처리를 하고싶다. (클라이언트에서만 처리)
	// 현재시간은 계속 흘러가야한다.
	// OnRep_RotYaw함수가 호출되면 최근시간에 현재시간값을 담는다. 현재시간을 다시 0으로 초기화한다.
	float currentTime;
	float latestTime;



	void SelfRotation( const float& DeltaTime );



	// 태어날 때 호출될 함수
	void ChangeMatColor();

	UPROPERTY()
	class UMaterialInstanceDynamic* mat;

	//UPROPERTY(ReplicatedUsing=OnRep_ChangeMatColor)
	UPROPERTY()
	FLinearColor matColor;

	//UFUNCTION()
	//void OnRep_ChangeMatColor(); // 색을 바꾸는 일을 하고싶다.

	// RPC : 함수 동기화
	// Server : 클라가 서버에게 Call
	// Client : 서버가 해당 클라에게 Call : Authority <-> AutonomousProxy
	// NetMulticast : 서버가 해당 클라들에게 Call : Authority <-> Any Proxy

	UFUNCTION(Server, Reliable)
	void ServerChangeMatColor( FLinearColor color);

	UFUNCTION( Client, Reliable )
	void ClientChangeMatColor( FLinearColor color );

	UFUNCTION( NetMulticast, Reliable )
	void MulticastChangeMatColor( FLinearColor color);



	virtual void GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const override;
};
