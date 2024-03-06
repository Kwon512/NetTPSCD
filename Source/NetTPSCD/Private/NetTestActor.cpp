// Fill out your copyright notice in the Description page of Project Settings.


#include "NetTestActor.h"

#include "EngineUtils.h"
#include "NetTPSCDCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ANetTestActor::ANetTestActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "meshComp" ) );
	SetRootComponent( meshComp );
	meshComp->SetRelativeScale3D( FVector( 0.5f ) );

	bReplicates = true;
}

// Called when the game starts or when spawned
void ANetTestActor::BeginPlay()
{
	Super::BeginPlay();

	NetUpdateFrequency = 100;

	ChangeMatColor();
}

// Called every frame
void ANetTestActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	PrintNetLog();

	FindOwner();

	SelfRotation( DeltaTime );

}


void ANetTestActor::PrintNetLog()
{
	// 오너가 있는가?
	FString owner = GetOwner() ? GetOwner()->GetName() : TEXT( "No Owner" );
	// NetConnection이 있는가?
	FString conn = GetNetConnection() ? TEXT( "Valid" ) : TEXT( "Invalid" );
	// LocalRole
	FString localRole = UEnum::GetValueAsString<ENetRole>( GetLocalRole() );
	// RemoteRole
	FString remoteRole = UEnum::GetValueAsString<ENetRole>( GetRemoteRole() );

	FString str = FString::Printf( TEXT( "Owner : %s\nConnection : %s\nlocalRole : %s\nremoteRole : %s" ) , *owner , *conn , *localRole , *remoteRole );

	FVector loc = GetActorLocation() + FVector( 0 , 0 , 50 );
	DrawDebugString( GetWorld() , loc , str , nullptr , FColor::Yellow , 0 , false , 0.75f );

}

void ANetTestActor::FindOwner()
{
	DrawDebugSphere( GetWorld() , GetActorLocation() , detectRadius , 32 , FColor::Cyan , false , 0 );

	// 레벨에 있는 ANetTPSCDCharacter 객체들을 다 검사해서 detectRadius안에 있고 그중에서도 가장 까가운녀석을 내 오너로 하고싶다.
	if (HasAuthority())
	{
		AActor* NewOwner = nullptr;
		float tempDist = detectRadius;
		for (TActorIterator<ANetTPSCDCharacter> It( GetWorld() ); It; ++It)
		{
			ANetTPSCDCharacter* player = *It;
			// 상대방과 나의 거리를 측정해서
			float temp = player->GetDistanceTo( this );
			// 만약 temp가 tempDist보다 가깝다면
			if (temp < tempDist)
			{
				// NewOwner로 기억하고싶다.
				NewOwner = player;
				// tempDist = temp;
				tempDist = temp;
			}
		}
		if (GetOwner() != NewOwner)
		{
			SetOwner( NewOwner );
		}
	}
}


void ANetTestActor::SelfRotation( const float& DeltaTime )
{
	// 회전
	// 만약 서버라면
	if (HasAuthority())
	{
		//  : 실제로 회전하고 그 결과를 rotYaw변수에 담고싶다.
		AddActorWorldRotation( FRotator( 0 , 360 * DeltaTime , 0 ) );
		rotYaw = GetActorRotation().Yaw;
	}
	// 그렇지않고 클라라면
	else
	{
		currentTime += DeltaTime;

		if (latestTime < KINDA_SMALL_NUMBER)
		{
			return;
		}

		// 비율값을 구하고싶다.
		float alpha = currentTime / latestTime;
		float newYaw = rotYaw + 360 * latestTime;
		// 시작값, 끝값
		float lerpYaw = FMath::Lerp<float>( rotYaw , newYaw , alpha );

		//  현재 회전값의 Yaw값을 rotYaw값으로 반영하고싶다.
		FRotator rot = GetActorRotation();
		rot.Yaw = lerpYaw;
		SetActorRotation( rot );
	}
}

// OnRep_RotYaw함수가 호출되면
void ANetTestActor::OnRep_RotYaw()
{
	// 최근시간에 현재시간값을 담는다.
	latestTime = currentTime;
	// 현재시간을 다시 0으로 초기화한다.
	currentTime = 0;
}

void ANetTestActor::ChangeMatColor()
{
	// 재질을 dynamic으로 다시 만들고 싶다.
	mat = meshComp->CreateDynamicMaterialInstance( 0 );

	// 서버에서
	if (HasAuthority())
	{
		// 타이머를 이용해서 1초마다 색을 바꾸고싶다.
		// 타이머로 람다식, 무명함수
		FTimerHandle handle;
		GetWorldTimerManager().SetTimer( handle , [&]()
		{
			matColor = FLinearColor::MakeRandomColor();
			//OnRep_ChangeMatColor();
			ServerChangeMatColor( matColor );
		} , 1 , true );
	}
}

//void ANetTestActor::OnRep_ChangeMatColor()
//{
//	if (mat)
//	{
//		mat->SetVectorParameterValue( TEXT( "FloorColor" ) , matColor );
//	}
//}

void ANetTestActor::ServerChangeMatColor_Implementation( FLinearColor color )
{
	MulticastChangeMatColor( color );
}

void ANetTestActor::ClientChangeMatColor_Implementation( FLinearColor color )
{
	if (mat)
	{
		mat->SetVectorParameterValue( TEXT( "FloorColor" ) , color );
	}
}

void ANetTestActor::MulticastChangeMatColor_Implementation(FLinearColor color)
{
	if (mat)
	{
		mat->SetVectorParameterValue( TEXT( "FloorColor" ) , color );
	}
}

void ANetTestActor::GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );

	DOREPLIFETIME( ANetTestActor , rotYaw );
	//DOREPLIFETIME( ANetTestActor , matColor );
}
