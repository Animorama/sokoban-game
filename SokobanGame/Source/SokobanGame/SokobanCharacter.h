// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "SokobanCharacter.generated.h"

UCLASS()
class SOKOBANGAME_API ASokobanCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASokobanCharacter();

	void SetEnabledMovement(bool bMovementEnabled);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	APlayerController* SokobanPlayerController;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* MappingContext;
	//Input Actions Declared here but Set in BP
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* QuitGameAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* ResetAction;

	UCharacterMovementComponent* MovementComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Animation")
	bool bIsPushing = false;
	const FVector Forward = FVector(1, 0, 0);
	const FVector Right = FVector(0, 1, 0);
	UPROPERTY(EditAnywhere, Category = "Movement")
	float EdgeOffsetDistance = 50.f;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float EdgeDepthCheckDistance = 100.f;

	void SetInitialMappingContext();
	void Move(const FInputActionValue& Value);
	void Quit(const FInputActionValue& Value);
	void Reset(const FInputActionValue& Value);
	bool EdgeInDirection(FVector Direction);

	UPROPERTY(EditAnywhere, Category = "Camera")
	TSubclassOf<class UCameraShakeBase> ResetCameraShakeClass;
};
