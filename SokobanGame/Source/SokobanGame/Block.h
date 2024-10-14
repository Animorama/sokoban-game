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

	////BP Event
	//UFUNCTION(BlueprintImplementableEvent, Category = "Custom")
	//void Push();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* BlockMeshComponent;

	class ASokobanCharacter* Character;

	//UFUNCTION() Needed for Callbacks to add to delegates
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	bool CanPush();
	bool HasObstacle();

	UPROPERTY(EditDefaultsOnly)
	float PushDistance = 100.f;
	UPROPERTY(EditDefaultsOnly)
	float PushTime = 1.f;

	bool bIsMoving = false;
	FVector TargetLocation = FVector::ZeroVector;
	FVector PushDirection = FVector::ZeroVector;

	void MoveBlock(float DeltaTime);
};
