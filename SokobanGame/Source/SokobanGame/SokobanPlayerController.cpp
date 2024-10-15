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
	Super::GameHasEnded(EndGameFocus, bIsWinner);

	//Handle Hud Logic
	UE_LOG(LogTemp, Warning, TEXT("Game Has Ended!"));


	GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, RestartDelay);
}