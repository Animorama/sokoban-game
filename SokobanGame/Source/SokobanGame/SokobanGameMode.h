// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SokobanGameMode.generated.h"
/**
 * 
 */

UCLASS()
class SOKOBANGAME_API ASokobanGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	void BlockPlaced(AActor* Block, AActor* Trigger);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Reference to the background music sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* BackgroundMusic;

	// Audio component to play the sound
	UPROPERTY()
	class UAudioComponent* AudioComponent;

	void PlayBackgroundMusic();

private:
	void EndGame();

	class ASokobanPlayerController* PlayerController;
};
