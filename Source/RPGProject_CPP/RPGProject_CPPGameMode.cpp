// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGProject_CPPGameMode.h"
#include "UObject/ConstructorHelpers.h"

ARPGProject_CPPGameMode::ARPGProject_CPPGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/BP_SwordCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
