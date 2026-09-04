// Copyright Epic Games, Inc. All Rights Reserved.

#include "FantasyProjectGameMode.h"
#include "FantasyProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFantasyProjectGameMode::AFantasyProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/PROJECT/Actors/PlayerOne/BP_PlayerWoman"));
	//if (PlayerPawnBPClass.Class != NULL)
	//{
		//DefaultPawnClass = PlayerPawnBPClass.Class;
	//}
}
