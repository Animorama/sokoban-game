// Fill out your copyright notice in the Description page of Project Settings.


#include "SokobanPlayerController.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"


void ASokobanPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//Create HUD
}

void ASokobanPlayerController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	////Super Call Messes up camera, maybe due to dispossessing pawn
	//Super::GameHasEnded(EndGameFocus, bIsWinner);

	//Handle Hud Logic
	UUserWidget* LevelClearedScreen = CreateWidget(this, LevelClearedScreenClass);

	if (LevelClearedScreen != nullptr)
	{
		LevelClearedScreen->AddToViewport();
	}


	GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, RestartDelay);
}