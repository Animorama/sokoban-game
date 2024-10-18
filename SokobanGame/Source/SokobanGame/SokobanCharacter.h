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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetEnabledMovement(bool bMovementEnabled);

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
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* RotateCameraAction;


	UPROPERTY(EditAnywhere, Category = "Camera")
	TSubclassOf<class UCameraShakeBase> ResetCameraShakeClass;

	UCharacterMovementComponent* MovementComponent;
	class UCameraComponent* CameraComponent;
	class USpringArmComponent* SpringArmComponent;

	FVector Forward = FVector(1, 0, 0);
	FVector Right = FVector(0, 1, 0);
	FVector CurrentDirection;
	FVector TargetLocation;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float MoveDistance = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float MoveTime = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float EdgeOffsetDistance = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float EdgeDepthCheckDistance = 100.f;

	//States
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Animation")
	bool bIsMoving = false;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Animation")
	bool bIsPushing = false;

	void SetInitialMappingContext();
	void HandleInput(const FInputActionValue& Value);
	void AlignForwardToCamera();
	bool IsValidMove(const FVector& Direction);
	void Movement(float DeltaTime);
	void Quit(const FInputActionValue& Value);
	void Reset(const FInputActionValue& Value);
	void RotateCamera(const FInputActionValue& Value);
	bool EdgeInDirection(const FVector& Direction);
	AActor* GetObstacleInDirection(const FVector& Direction);

};
