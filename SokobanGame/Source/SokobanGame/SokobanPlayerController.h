// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SokobanPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SOKOBANGAME_API ASokobanPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void GameHasEnded(class AActor* EndGameFocus = nullptr, bool bIsWinner = false) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> LevelClearedScreenClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> HUDClass;
	UPROPERTY()
	UUserWidget* HUD;

	UPROPERTY(EditAnywhere)
	float RestartDelay = 3.f;
	FTimerHandle RestartTimer;
};
