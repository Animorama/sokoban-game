// Fill out your copyright notice in the Description page of Project Settings.


#include "SokobanGameMode.h"
#include "EngineUtils.h"
#include "BoxTrigger.h"
#include "SokobanPlayerController.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

void ASokobanGameMode::BeginPlay()
{
	Super::BeginPlay();

	//Music Restarts when resetting level, this should be in GameInstance class???
	PlayBackgroundMusic();
}

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


void ASokobanGameMode::PlayBackgroundMusic()
{
	if (BackgroundMusic)
	{
		// Create the AudioComponent and attach it to root
		AudioComponent = NewObject<UAudioComponent>(this);
		AudioComponent->bAutoActivate = false;
		AudioComponent->SetSound(BackgroundMusic);
		AudioComponent->bIsUISound = true;
		AudioComponent->bOverrideAttenuation = true;
		AudioComponent->Play();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Background music sound is not set!"));
	}
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
