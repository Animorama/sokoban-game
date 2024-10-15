// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "BoxTrigger.generated.h"

/**
 * 
 */
UCLASS()
class SOKOBANGAME_API ABoxTrigger : public ATriggerBox
{
	GENERATED_BODY()
	
public:
	ABoxTrigger();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);
	UFUNCTION()
	void OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor);

	bool IsOccupied() const { return bIsOccupied; };
	////Could use it for SpecificActor pointer such as Character
	//UPROPERTY(EditAnywhere)
	//class AActor* SpecificActor;
private:
	bool bIsOccupied;
	class ASokobanGameMode* GameMode;

};
