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
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ANetTPSCDCharacter

ANetTPSCDCharacter::ANetTPSCDCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bUseControllerDesiredRotation= true;
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 130.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SetRelativeLocation(FVector(0, 40, 60));

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// 손 컴포넌트를 생성해서 GetMesh의 GunPoint에 붙이고싶다.
	handComp = CreateDefaultSubobject<USceneComponent>(TEXT("handComp"));
	handComp->SetupAttachment(GetMesh(), TEXT("GunPoint"));
	handComp->SetRelativeLocationAndRotation(
		FVector(-16.117320f, 2.606926f, 3.561379f),
		FRotator(17.690681f, 83.344357f,  9.577745));
}

void ANetTPSCDCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ANetTPSCDCharacter::PickupPistol(const FInputActionValue& Value)
{
	if (bHasPistol)
		return;

	// 가까운 총을 검색해서 
	TArray<struct FOverlapResult> OutOverlaps;
	FCollisionObjectQueryParams ObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects);

	bool bHits = GetWorld()->OverlapMultiByObjectType(
		OutOverlaps,
		GetActorLocation(),
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(findPistolRadius));

	// 만약 검색된 결과 있다면
	if (bHits)
	{
		// 전체 검색해서
		for (auto result : OutOverlaps)
		{
			// 만약 액터의 이름에 BP_Pistol이 포함되어있다면
			if (result.GetActor()->GetActorNameOrLabel().Contains(TEXT("BP_Pistol")))
			{
				// 그것을 grabPistol로 하고싶다.
				grabPistol = result.GetActor();
				// 반복을 그만하고싶다.
				break;
			}
		}
	}

	// grabPistol이 nullptr이 아니라면 손에 붙이고싶다.
	if (grabPistol)
	{
		AttachPistol(grabPistol);
		bHasPistol = true;
	}
}

void ANetTPSCDCharacter::DropPistol(const FInputActionValue& Value)
{
	if (false == bHasPistol)
		return;

	bHasPistol = false;
	DetachPistol(grabPistol);
}

void ANetTPSCDCharacter::AttachPistol(const AActor* pistol)
{
	// pistol의 staticmeshcomponent를 가져오고싶다.
	auto mesh = pistol->GetComponentByClass<UStaticMeshComponent>();
	// pistol 물리를 끄고싶다.
	mesh->SetSimulatePhysics(false);
	// hand에 붙이고싶다.
	mesh->AttachToComponent(handComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void ANetTPSCDCharacter::DetachPistol(const AActor* pistol)
{
	if (nullptr == grabPistol)
		return;

	// pistol의 staticmeshcomponent를 가져오고싶다.
	auto mesh = pistol->GetComponentByClass<UStaticMeshComponent>();
	// pistol 물리를 켜고싶다.
	mesh->SetSimulatePhysics(true);
	// hand에서 떼고싶다.
	mesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);

	grabPistol = nullptr;
}


//////////////////////////////////////////////////////////////////////////
// Input

void ANetTPSCDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANetTPSCDCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANetTPSCDCharacter::Look);

		EnhancedInputComponent->BindAction(PickupPistolAction, ETriggerEvent::Started, this, &ANetTPSCDCharacter::PickupPistol);

		EnhancedInputComponent->BindAction(DropPistolAction, ETriggerEvent::Started, this, &ANetTPSCDCharacter::DropPistol);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ANetTPSCDCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ANetTPSCDCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}