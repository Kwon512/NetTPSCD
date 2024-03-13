// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetTPSCDCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HPBarWidget.h"
#include "InputActionValue.h"
#include "MainUI.h"
#include "NetPlayerAnimInstance.h"
#include "NetPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY( LogTemplateCharacter );
DEFINE_LOG_CATEGORY( MyLog );

//////////////////////////////////////////////////////////////////////////
// ANetTPSCDCharacter

ANetTPSCDCharacter::ANetTPSCDCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize( 42.f , 96.0f );

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator( 0.0f , 500.0f , 0.0f ); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>( TEXT( "CameraBoom" ) );
	CameraBoom->SetupAttachment( RootComponent );
	CameraBoom->TargetArmLength = 130.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SetRelativeLocation( FVector( 0 , 40 , 60 ) );

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>( TEXT( "FollowCamera" ) );
	FollowCamera->SetupAttachment( CameraBoom , USpringArmComponent::SocketName ); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// 손 컴포넌트를 생성해서 GetMesh의 GunPoint에 붙이고싶다.
	handComp = CreateDefaultSubobject<USceneComponent>( TEXT( "handComp" ) );
	handComp->SetupAttachment( GetMesh() , TEXT( "GunPoint" ) );
	handComp->SetRelativeLocationAndRotation(
		FVector( -16.117320f , 2.606926f , 3.561379f ) ,
		FRotator( 17.690681f , 83.344357f , 9.577745 ) );

	// 상대방의 hpUIComp 컴포넌트를 추가하고싶다.
	hpUIComp = CreateDefaultSubobject<UWidgetComponent>( TEXT( "hpUIComp" ) );
	hpUIComp->SetupAttachment( RootComponent );

	bReplicates = true;
	SetReplicateMovement( true );
}

void ANetTPSCDCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	InitUI();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>( Controller ))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>( PlayerController->GetLocalPlayer() ))
		{
			Subsystem->AddMappingContext( DefaultMappingContext , 0 );
		}
	}
}

void ANetTPSCDCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitUI();
}

void ANetTPSCDCharacter::Tick( float DeltaSeconds )
{
	Super::Tick( DeltaSeconds );

	PrintNetLog();

	// hpUIComp를 빌보드 처리 하고싶다.
	if (hpUIComp && hpUIComp->GetVisibleFlag())
	{
		auto cam = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
		FVector dir = cam->GetCameraLocation() - hpUIComp->GetComponentLocation();

		hpUIComp->SetWorldRotation( dir.GetSafeNormal2D().ToOrientationRotator() );
	}
}

void ANetTPSCDCharacter::InitUI()
{
	// 태어날 때 hpUI를 가져오고싶다.
	if (nullptr == hpUI)
	{
		hpUI = Cast<UHPBarWidget>( hpUIComp->GetWidget() );
	}

	// 컨트롤러가 PlayerController가 아니라면 함수를 바로 종료
	// 즉, mainUI를 생성하지 않겠다.
	auto pc = Cast<ANetPlayerController>( Controller );
	if (nullptr == pc)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s : nullptr == pc"), __FUNCTION__);
		return;
	}
	if (false == IsLocallyControlled())
	{
		UE_LOG( LogTemp , Warning , TEXT( "%s : false == IsLocallyControlled()" ) , __FUNCTION__ );
		return;
	}
	UE_LOG( LogTemp , Warning , TEXT( "ANetTPSCDCharacter::InitUI" ) );

	// mainUI를 생성한다면 hpUIComp를 비활성화 하고싶다.
	hpUIComp->SetVisibility( false );

	if (nullptr == pc->mainUI)
	{
		// MainUI를 생성해서 기억하고싶다.
		pc->mainUI = CreateWidget<UMainUI>( GetWorld() , pc->mainUIFactory );
		// AddToViewport하고싶다.
		pc->mainUI->AddToViewport();
		// 크로스헤어를 안보이게 하고싶다.
		pc->mainUI->SetActiveCrosshair( false );
	}

	// 만들어진 mainUI를 기억하고싶다.
	mainUI = pc->mainUI;

	// 체력을 초기화하고싶다.
	// 총알UI를 최대갯수로 초기화하고싶다.
	if (mainUI)
	{
		mainUI->hp = 1.0f;
		mainUI->ReloadBulletUI( maxBulletCount );
	}
}

void ANetTPSCDCharacter::PickupPistol( const FInputActionValue& Value )
{
	if (bHasPistol || isReload)
		return;

	// 가까운 총을 검색해서 
	TArray<struct FOverlapResult> OutOverlaps;
	FCollisionObjectQueryParams ObjectQueryParams( FCollisionObjectQueryParams::InitType::AllObjects );

	bool bHits = GetWorld()->OverlapMultiByObjectType(
		OutOverlaps ,
		GetActorLocation() ,
		FQuat::Identity ,
		ObjectQueryParams ,
		FCollisionShape::MakeSphere( findPistolRadius ) );

	AActor* _tempGrabPistol = nullptr;
	// 만약 검색된 결과 있다면
	if (bHits)
	{
		// 전체 검색해서
		for (auto result : OutOverlaps)
		{
			// 만약 액터의 오너가 없고 액터의 이름에 BP_Pistol이 포함되어있다면
			AActor* _temp = result.GetActor();
			if (nullptr == _temp->GetOwner() &&
				_temp->GetActorNameOrLabel().Contains( TEXT( "BP_Pistol" ) ))
			{
				// 그것을 grabPistol로 하고싶다.
				_tempGrabPistol = result.GetActor();
				// 반복을 그만하고싶다.
				break;
			}
		}
	}

	// 만약 _tempGrabPistol이 nullptr이 아니라면
	// 서버에게 손에 붙여달라고 요청하고싶다.
	if (_tempGrabPistol)
	{
		ServerAttachPistol( _tempGrabPistol );
	}
}

void ANetTPSCDCharacter::ServerAttachPistol_Implementation( AActor* pistol )
{
	MultiAttachPistol( pistol );
}

void ANetTPSCDCharacter::MultiAttachPistol_Implementation( AActor* pistol )
{
	grabPistol = pistol;
	AttachPistol( pistol );
	grabPistol->SetOwner( this );
	bHasPistol = true;
	isReload = false;
	if (mainUI)
	{
		mainUI->SetActiveCrosshair( true );
	}
}
bool ANetTPSCDCharacter::ServerDetachPistol_Validate( AActor* pistol )
{
	return true;
}


void ANetTPSCDCharacter::DropPistol( const FInputActionValue& Value )
{
	if (false == bHasPistol || isReload)
		return;

	ServerDetachPistol( grabPistol );
}
void ANetTPSCDCharacter::ServerDetachPistol_Implementation( AActor* pistol )
{
	MultiDetachPistol( pistol );
}
void ANetTPSCDCharacter::MultiDetachPistol_Implementation( AActor* pistol )
{
	bHasPistol = false;
	DetachPistol( pistol );
	if (mainUI)
		mainUI->SetActiveCrosshair( false );
}

void ANetTPSCDCharacter::AttachPistol( const AActor* pistol )
{
	// pistol의 staticmeshcomponent를 가져오고싶다.
	auto mesh = pistol->GetComponentByClass<UStaticMeshComponent>();
	// pistol 물리를 끄고싶다.
	mesh->SetSimulatePhysics( false );
	// hand에 붙이고싶다.
	mesh->AttachToComponent( handComp , FAttachmentTransformRules::SnapToTargetNotIncludingScale );
}

void ANetTPSCDCharacter::DetachPistol( const AActor* pistol )
{
	if (nullptr == grabPistol)
		return;

	// pistol의 staticmeshcomponent를 가져오고싶다.
	auto mesh = pistol->GetComponentByClass<UStaticMeshComponent>();
	// pistol 물리를 켜고싶다.
	mesh->SetSimulatePhysics( true );
	// hand에서 떼고싶다.
	mesh->DetachFromComponent( FDetachmentTransformRules::KeepRelativeTransform );

	grabPistol->SetOwner( nullptr );
	grabPistol = nullptr;
}

void ANetTPSCDCharacter::Fire( const FInputActionValue& Value )
{
	// 내가 총을 가지고 있지 않다면 바로 함수 종료
	// bulletCount가 0 이하라면 바로 함수 종료
	if (false == bHasPistol || nullptr == grabPistol || bulletCount <= 0)
		return;

	// 만약 재장전 중이라면 함수를 바로 종료
	if (isReload)
		return;

	ServerFire();

}


void ANetTPSCDCharacter::ServerFire_Implementation()
{
	// - 카메라위치에서 카메라 앞방향으로
	FHitResult OutHit;
	FVector Start = FollowCamera->GetComponentLocation();
	FVector End = Start + FollowCamera->GetForwardVector() * 100000;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor( this );
	// 바라보고
	bool bHit = GetWorld()->LineTraceSingleByChannel( OutHit , Start , End , ECollisionChannel::ECC_Visibility , Params );

	if (bHit)
	{
		// 만약 부딪힌 상대방이 ANetTPSCDCharacter라면
		// TakeDamage로 데미지를 1점 주고싶다.
		auto otherPlayer = Cast<ANetTPSCDCharacter>( OutHit.GetActor() );
		if (otherPlayer)
		{
			otherPlayer->TakeDamage( 1 );
		}
	}

	MultiFire( bHit , OutHit , bulletCount - 1 );

}

void ANetTPSCDCharacter::MultiFire_Implementation( bool bHit , const FHitResult& hitInfo , int32 newBulletCount )
{
	// 1개 차감하고
	bulletCount = newBulletCount;
	// 총알UI를 갱신하고싶다.
	if (mainUI)
	{
		mainUI->RemoveBulletUI( bulletCount );
	}

	// UNetPlayerAnimInstance::PlayerFireAnimation를 호출하고싶다.
	// 1. UNetPlayerAnimInstance를 가져오고싶다.
	auto anim = Cast<UNetPlayerAnimInstance>( GetMesh()->GetAnimInstance() );
	// 2. PlayerFireAnimation를 호출하고싶다.
	anim->PlayFireAnimation();


	// 만약 부딪힌곳이 있다면 
	if (bHit)
	{
		// 그곳에 폭발VFX를 배치하고싶다.
		UGameplayStatics::SpawnEmitterAtLocation( GetWorld() , ExplosionVFXFactory , hitInfo.ImpactPoint );


	}
}

void ANetTPSCDCharacter::Reload( const FInputActionValue& Value )
{
	// 만약 재장전 중이라면 함수를 바로 종료
	if (isReload)
		return;

	ServerReload();
}

void ANetTPSCDCharacter::ServerReload_Implementation()
{
	MultiReload();
}

void ANetTPSCDCharacter::MultiReload_Implementation()
{
	isReload = true;
	// 리로드 애니메이션을 재생.
	auto anim = Cast<UNetPlayerAnimInstance>( GetMesh()->GetAnimInstance() );
	anim->PlayReloadAnimation();
}

void ANetTPSCDCharacter::ServerInitAmmo_Implementation()
{
	bulletCount = maxBulletCount;
	MultiInitAmmo();
}

void ANetTPSCDCharacter::MultiInitAmmo_Implementation()
{
	InitAmmo();
}


void ANetTPSCDCharacter::InitAmmo()
{
	if (mainUI)
	{
		mainUI->ReloadBulletUI( maxBulletCount );
	}
	isReload = false;
}


void ANetTPSCDCharacter::OnRep_HP()
{
	// UI도 반영하고싶다.
	float newHP = static_cast<float>(hp) / maxHP;
	if (mainUI) // 내꺼
	{
		mainUI->hp = newHP;
		mainUI->PlayHitAnim();

	}
	else // 니꺼
	{
		hpUI->hp = newHP;
	}

}

int32 ANetTPSCDCharacter::GetHP()
{
	return hp;
}

// 서버에서 호출됨.
void ANetTPSCDCharacter::SetHP( int32 value )
{
	hp = value;

	if (hp <= 0)
	{
		bDie = true;
		// 총을 놓고싶다.
		//DropPistol(FInputActionValue());
		if (grabPistol)
			MultiDetachPistol( grabPistol );

		// 이동을 막고싶다.
		GetCharacterMovement()->DisableMovement();
	}

	OnRep_HP();
}

void ANetTPSCDCharacter::TakeDamage( int32 damage )
{
	// 데미지만큼 체력을 감소하고싶다.
	int32 newHP = FMath::Clamp<int32>( GetHP() - damage , 0 , maxHP );
	SetHP( newHP );
}

void ANetTPSCDCharacter::PrintNetLog()
{
	// 오너가 있는가?
	FString owner = GetOwner() ? GetOwner()->GetName() : TEXT( "No Owner" );
	// NetConnection이 있는가?
	FString conn = GetNetConnection() ? TEXT( "Valid" ) : TEXT( "Invalid" );
	// LocalRole
	FString localRole = UEnum::GetValueAsString<ENetRole>( GetLocalRole() );
	// RemoteRole
	FString remoteRole = UEnum::GetValueAsString<ENetRole>( GetRemoteRole() );

	FString locallyController = IsLocallyControlled() ? TEXT( "Yes" ) : TEXT( "No" );

	FString str = FString::Printf( TEXT( "Owner : %s\nConnection : %s\nlocalRole : %s\nremoteRole : %s\nlocallyController : %s" ) , *owner , *conn , *localRole , *remoteRole, *locallyController );

	FVector loc = GetActorLocation() + FVector( 0 , 0 , 50 );
	DrawDebugString( GetWorld() , loc , str , nullptr , FColor::Yellow , 0 , false , 0.75f );

}



//////////////////////////////////////////////////////////////////////////
// Input

void ANetTPSCDCharacter::SetupPlayerInputComponent( UInputComponent* PlayerInputComponent )
{
	UE_LOG( MyLog , Warning , TEXT( "ANetTPSCDCharacter::SetupPlayerInputComponent" ) );

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>( PlayerInputComponent )) {

		// Jumping
		EnhancedInputComponent->BindAction( JumpAction , ETriggerEvent::Started , this , &ACharacter::Jump );
		EnhancedInputComponent->BindAction( JumpAction , ETriggerEvent::Completed , this , &ACharacter::StopJumping );

		// Moving
		EnhancedInputComponent->BindAction( MoveAction , ETriggerEvent::Triggered , this , &ANetTPSCDCharacter::Move );

		// Looking
		EnhancedInputComponent->BindAction( LookAction , ETriggerEvent::Triggered , this , &ANetTPSCDCharacter::Look );

		EnhancedInputComponent->BindAction( PickupPistolAction , ETriggerEvent::Started , this , &ANetTPSCDCharacter::PickupPistol );

		EnhancedInputComponent->BindAction( DropPistolAction , ETriggerEvent::Started , this , &ANetTPSCDCharacter::DropPistol );

		EnhancedInputComponent->BindAction( FireAction , ETriggerEvent::Started , this , &ANetTPSCDCharacter::Fire );

		EnhancedInputComponent->BindAction( ReloadAction , ETriggerEvent::Started , this , &ANetTPSCDCharacter::Reload );
	}
	else
	{
		UE_LOG( LogTemplateCharacter , Error , TEXT( "'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file." ) , *GetNameSafe( this ) );
	}
}

void ANetTPSCDCharacter::Move( const FInputActionValue& Value )
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation( 0 , Rotation.Yaw , 0 );

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix( YawRotation ).GetUnitAxis( EAxis::X );

		// get right vector 
		const FVector RightDirection = FRotationMatrix( YawRotation ).GetUnitAxis( EAxis::Y );

		// add movement 
		AddMovementInput( ForwardDirection , MovementVector.Y );
		AddMovementInput( RightDirection , MovementVector.X );
	}
}

void ANetTPSCDCharacter::Look( const FInputActionValue& Value )
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput( LookAxisVector.X );
		AddControllerPitchInput( LookAxisVector.Y );
	}
}


void ANetTPSCDCharacter::DamageProcess()
{
	// 죽음 애니메이션이 끝나면
	// 마우스 커서를 보이게하고싶다.
	auto pc = GetWorld()->GetFirstPlayerController();
	pc->SetShowMouseCursor( true );
	// 화면을 회색으로 보이게 하고싶다.
	FollowCamera->PostProcessSettings.ColorSaturation = FVector4( 0 , 0 , 0 , 1 );
	// 게임오버UI를 보이게하고싶다.
	if (mainUI)
	{
		mainUI->SetShowGameOverUI( true );
	}
}

void ANetTPSCDCharacter::GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );

	DOREPLIFETIME( ANetTPSCDCharacter , bHasPistol );
	DOREPLIFETIME( ANetTPSCDCharacter , bulletCount );
	DOREPLIFETIME( ANetTPSCDCharacter , hp );
	DOREPLIFETIME( ANetTPSCDCharacter , bDie );
}
