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

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* meshComp;

	void PrintNetLog();

	UPROPERTY(EditDefaultsOnly)
	float detectRadius = 300.0f;

	void FindOwner();


	// 변수 리플리케이트
	// 함수 리플리케이트 : (RPC, Remote Procedure Call)

	// 속성 리플리케이트
	UPROPERTY(Replicated)
	float rotYaw;

	void SelfRotation( const float& DeltaTime );

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
