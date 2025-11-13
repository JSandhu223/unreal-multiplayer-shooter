// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"


void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = this->GameState.Get()->PlayerArray.Num();
	if (NumberOfPlayers == 2)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			this->bUseSeamlessTravel = true;
			// Travel to the BlasterMap, specifying it as a listen server for clients to connect to
			World->ServerTravel(FString("/Game/Maps/BlasterMap?listen"));
		}
	}
}
