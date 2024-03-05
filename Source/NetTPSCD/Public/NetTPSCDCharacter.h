// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "NetTPSCDCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ANetTPSCDCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

public:
	ANetTPSCDCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void InitUI();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


	// --------------------------------------------
protected:

	void PickupPistol(const FInputActionValue& Value);
	void DropPistol(const FInputActionValue& Value);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bHasPistol = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PickupPistolAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DropPistolAction;

	// 손에 해당하는 컴포넌트를 만들어서 손 소켓에 붙이고싶다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pistol)
	class USceneComponent* handComp;

	// 총을 잡을 수 있는 검색 거리
	UPROPERTY(EditDefaultsOnly, Category = Pistol)
	float findPistolRadius = 150;

	// 잡은 총 액터
	UPROPERTY()
	class AActor* grabPistol;

	// 총을 손에 붙이는 기능
	void AttachPistol(const AActor* pistol);
	// 총을 손에서 떼는 기능
	void DetachPistol(const AActor* pistol);

	// 마우스 왼쪽 버튼을 클릭하면
	// 총을 쏘고싶다. 부딪힌것이 있다면 그곳에 폭발VFX를 표현하고싶다.
	// - 입력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;

	void Fire( const FInputActionValue& Value );

	// - 폭발VFX공장
	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* ExplosionVFXFactory;

	// MainUI를 화면에 보이게하고 싶다.
	// BeginPlay에서 MainUI를 생성해서 기억하고 AddToViewport하고싶다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UMainUI> mainUIFactory;

	UPROPERTY()
	class UMainUI* mainUI;

	UPROPERTY(EditDefaultsOnly)
	int32 maxBulletCount = 21;

	int32 bulletCount = maxBulletCount;

	UPROPERTY( EditAnywhere , BlueprintReadOnly , Category = Input )
	UInputAction* ReloadAction;

	void Reload( const FInputActionValue& Value );

	void InitAmmo();
	// 재장전 중에
	// 재장전을 막고싶다.
	// 총쏘기도 막고싶다.
	bool isReload;

	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly )
	int32 maxHP = 3;

	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly )
	int32 hp = maxHP;

	// hp를 property를 이용해서 접근하고싶다.
	__declspec(property(get = GetHP , put = SetHP)) int32 HP;

	int32 GetHP();

	void SetHP( int32 value );

	void TakeDamage( int32 damage );

	// 상대방의 HUD를 추가하고싶다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UWidgetComponent* hpUIComp;

	UPROPERTY()
	class UHPBarWidget* hpUI;

	UPROPERTY(EditDefaultsOnly)
	bool bDie = false;

	void PrintNetLog();

};

