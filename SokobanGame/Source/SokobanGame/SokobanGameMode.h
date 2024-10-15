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

private:
	void EndGame();

	class ASokobanPlayerController* PlayerController;
};
