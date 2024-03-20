// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MessageUI.generated.h"

/**
 * 
 */
UCLASS()
class NETTPSCD_API UMessageUI : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	class UTextBlock* txt_msg;
};
