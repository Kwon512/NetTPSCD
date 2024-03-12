// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NetPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class NETTPSCD_API UNetPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY()
	class ANetTPSCDCharacter* player;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bHasPistol = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float direction = false;

	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* fireMontage;

	void PlayFireAnimation();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float pitchAngle;

	// Reload 애니메이션을 재생하는 기능을 구현하고싶다.
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* reloadMontage;

	void PlayReloadAnimation();

	// Reload가 끝났을 때 호출될 이벤트함수를 구현하고싶다.
	UFUNCTION()
	void AnimNotify_OnReloadFinished();

	UPROPERTY( EditDefaultsOnly , BlueprintReadOnly)
	bool bDie = false;

	UFUNCTION()
	void AnimNotify_DieEnd();
};
