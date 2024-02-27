// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetTPSCDGameMode.h"
#include "NetTPSCDCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANetTPSCDGameMode::ANetTPSCDGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
