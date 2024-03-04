// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HPBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class NETTPSCD_API UHPBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY( EditDefaultsOnly , meta = (BindWidget) )
	class UProgressBar* bar_hp;

	UPROPERTY( EditDefaultsOnly , BlueprintReadOnly )
	float hp = 1.0f;
};
