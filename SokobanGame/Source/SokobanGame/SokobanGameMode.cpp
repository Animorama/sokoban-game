// Fill out your copyright notice in the Description page of Project Settings.


#include "SokobanGameMode.h"
#include "EngineUtils.h"
#include "BoxTrigger.h"
#include "SokobanPlayerController.h"

void ASokobanGameMode::BlockPlaced(AActor* Block, AActor* Trigger)
{
	for (ABoxTrigger* Trigger : TActorRange<ABoxTrigger>(GetWorld()))
	{
		if (!Trigger->IsOccupied())
		{
			return;
		}
	}
	EndGame();
}

void ASokobanGameMode::EndGame()
{
	PlayerController = Cast<ASokobanPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController)
	{
		PlayerController->GameHasEnded();
	}
	//Notify Player Controller for hud, game restart etc.
}
