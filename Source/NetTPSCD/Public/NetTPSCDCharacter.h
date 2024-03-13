// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
// #pragma warning(disable:4458)

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
DECLARE_LOG_CATEGORY_EXTERN( MyLog , Log , All );

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

	virtual void PossessedBy(AController* NewController) override;

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
	UPROPERTY(Replicated)
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

	UPROPERTY()
	class UMainUI* mainUI;

	UPROPERTY(EditDefaultsOnly)
	int32 maxBulletCount = 21;

	UPROPERTY(Replicated)
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

	UPROPERTY(ReplicatedUsing=OnRep_HP, EditDefaultsOnly, BlueprintReadOnly )
	int32 hp = maxHP;

	UFUNCTION()
	void OnRep_HP();

	// hp를 property를 이용해서 접근하고싶다.
	//__declspec(property(get = GetHP , put = SetHP)) int32 HP;

	int32 GetHP();

	void SetHP( int32 value );

	void TakeDamage( int32 damage );

	// 상대방의 HUD를 추가하고싶다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UWidgetComponent* hpUIComp;

	UPROPERTY()
	class UHPBarWidget* hpUI;

	UPROPERTY(Replicated, EditDefaultsOnly)
	bool bDie = false;


	// Network ----------------------------------------------

	void PrintNetLog();

	// 클라2서버 손에 붙여주세요(총액터의 포인터)
	UFUNCTION(Server, Reliable)
	void ServerAttachPistol( AActor* pistol ); // 요청
	//void ServerAttachPistol_Implementation( AActor* pistol ); // 응답

	// 서버2멀티 손에 붙이세요(총액터의 포인터)
	UFUNCTION(NetMulticast, Reliable)
	void MultiAttachPistol( AActor* pistol );

	// 손에서 총을 놓고싶다.
	// 클라2서버 총을 놓아주세요(총액의 포인터)
	UFUNCTION( Server , Reliable, WithValidation )
	void ServerDetachPistol( AActor* pistol );
	// 서버2멀티 모두 총을 놓으세요(총액의 포인터)
	UFUNCTION( NetMulticast , Reliable )
	void MultiDetachPistol( AActor* pistol );


	// 사용자가 총을 쏘면
	// 서버에게 총을 쏴 달라고하고싶다.
	// 서버에서 라인을 그려서 부딪힌것이 있다면
	// 그 정보를 모든 클라이언트에게 보내서 총쏘기 처리를 하고싶다.
	UFUNCTION( Server , Reliable  )
	void ServerFire();
	
	UFUNCTION( NetMulticast , Reliable )
	void MultiFire(bool bHit, const FHitResult& hitInfo, int32 newBulletCount );


	// 클라2서버 재장전 애니메이션을 요청
	UFUNCTION( Server , Reliable )
	void ServerReload();
	// 서버2멀티 재장전애니메이션 해라
	UFUNCTION( NetMulticast , Reliable )
	void MultiReload();

	// 클라2서버 initAmmo를 해주세요.
	UFUNCTION( Server , Reliable )
	void ServerInitAmmo();
	// 서버2멀티 initAmmo를 해라
	UFUNCTION( NetMulticast , Reliable )
	void MultiInitAmmo();
	// 애니메이션이 끝나면 ServerInitAmmo 가 불리면

	void DamageProcess();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


};

