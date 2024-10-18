// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Block.generated.h"

UCLASS()
class SOKOBANGAME_API ABlock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool CanBePushed(FVector Direction);
	void PushBlock();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UBoxComponent* BoxComponent;
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* PushSound;

	class ASokobanCharacter* Character;

	UPROPERTY(EditDefaultsOnly)
	float PushDistance = 100.f;
	UPROPERTY(EditDefaultsOnly)
	float PushTime = 0.2f;

	bool bIsMoving = false;
	bool bIsFalling = false;
	bool bBeyondEdge = false;
	FVector TargetLocation = FVector::ZeroVector;
	FVector PushDirection = FVector::ZeroVector;

	void Move(float DeltaTime);
	void Fall(float DeltaTime);
};
