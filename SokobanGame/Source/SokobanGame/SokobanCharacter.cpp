// Fill out your copyright notice in the Description page of Project Settings.


#include "SokobanCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "SokobanPlayerController.h"
#include "Block.h"

// Sets default values
ASokobanCharacter::ASokobanCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ASokobanCharacter::SetEnabledMovement(bool bMovementEnabled)
{
	if (MovementComponent)
	{
		if (bMovementEnabled)
		{
			MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);

			//REFACTOR -> This should not be here?
			bIsPushing = false;
		}
		else
		{
			MovementComponent->DisableMovement();

			//REFACTOR -> This should not be here?
			bIsMoving = false;
			bIsPushing = false;
		}
	}
}

// Called when the game starts or when spawned
void ASokobanCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetInitialMappingContext();
	MovementComponent = GetCharacterMovement();
	CameraComponent = Cast<UCameraComponent>(GetComponentByClass(UCameraComponent::StaticClass()));
	SpringArmComponent = Cast<USpringArmComponent>(GetComponentByClass(USpringArmComponent::StaticClass()));
	AlignForwardToCamera();
}

// Called every frame
void ASokobanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMoving)
	{
		Movement(DeltaTime);
	}
}

void ASokobanCharacter::HandleInput(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (bIsMoving || bIsPushing)
	{
		return;
	}

	//Prevent diagonal movement, refactor
	if (MovementVector.X != 0.f && MovementVector.Y != 0.f)
	{
		return;
	}

	if (MovementVector.IsNearlyZero())
	{
		return;
	}

	//AlignForwardToCamera();

	MovementVector.X = FMath::Sign(MovementVector.X);
	MovementVector.Y = FMath::Sign(MovementVector.Y);

	//Scale movement using input vector
	FVector WorldDirection = (Forward * MovementVector.Y) + (Right * MovementVector.X);
	WorldDirection.Z = 0.f;
	WorldDirection.Normalize();

	if (IsValidMove(WorldDirection))
	{
		//AddMovementInput(WorldDirection);
		TargetLocation = GetActorLocation() + WorldDirection * MoveDistance;
		CurrentDirection = WorldDirection;
		bIsMoving = true;
	}

	////Draw Debug Line Gizmo Of Actor Forward
	//FVector Start = GetActorLocation();
	//FVector End = Start + WorldDirection * 100.f;
	//DrawDebugLine(GetWorld(), Start, End, FColor::Red);
}

bool ASokobanCharacter::IsValidMove(const FVector& Direction)
{

	//Check for edges
	if (EdgeInDirection(Direction))
	{
		return false;
	}
	//Check for obstacles in direction
	AActor* ObstacleActor = GetObstacleInDirection(Direction);
	if (ObstacleActor == nullptr)
	{
		return true;
	}

	//Push if obstacle is a Movable Block
	if (ObstacleActor->IsA(ABlock::StaticClass()))
	{
		ABlock* MovableBlock = Cast<ABlock>(ObstacleActor);
		if (MovableBlock->CanBePushed(Direction))
		{
			MovableBlock->PushBlock();
			bIsPushing = true;
			return true;
		}
	}

	return false;
}

AActor* ASokobanCharacter::GetObstacleInDirection(const FVector& Direction)
{
	if (UWorld* World = GetWorld())
	{
		FVector Start = GetActorLocation();
		FVector End = Start + Direction * MoveDistance;
		FHitResult HitResult;

		bool bHit = World->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);

		//DrawDebugLine(World, Start, End, FColor::Black, false, 2.0f);

		if (bHit)
		{
			return HitResult.GetActor();
		}
	}
	return nullptr;
}

bool ASokobanCharacter::EdgeInDirection(const FVector& Direction)
{
	if (UWorld* World = GetWorld())
	{
		FVector Start = GetActorLocation() + Direction * EdgeOffsetDistance;
		FVector End = Start - FVector(0.f, 0.f, EdgeDepthCheckDistance);
		FHitResult EdgeHitResult;

		bool bFloorHit = World->LineTraceSingleByChannel(EdgeHitResult, Start, End, ECollisionChannel::ECC_Visibility);

		if (bFloorHit)
		{
			return false;
		}
	}
	return true;
}

void ASokobanCharacter::Movement(float DeltaTime)
{
	FVector CurrentLocation = GetActorLocation();
	float MovementSpeed = (CurrentDirection * MoveDistance).Length() / MoveTime;

	//Move
	FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, MovementSpeed);
	SetActorLocation(NewLocation);

	//Stop moving
	if (FVector::Dist(CurrentLocation, TargetLocation) < 10)
	{
		SetActorLocation(TargetLocation);
		bIsMoving = false;

		if (bIsPushing)
		{
			bIsPushing = false;
		}
	}

	// Calculate target rotation using direction vector
	FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(CurrentDirection);
	FRotator CurrentRotation = GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 10.0f);

	SetActorRotation(NewRotation);
}

void ASokobanCharacter::Quit(const FInputActionValue& Value)
{
	UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Type::Quit, false);
}

void ASokobanCharacter::Reset(const FInputActionValue& Value)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		PlayerController->RestartLevel();
		//CameraShake not looking good due to RestartLevel()
		if (ResetCameraShakeClass)
		{
			PlayerController->ClientStartCameraShake(ResetCameraShakeClass);
		}
	}
}

void ASokobanCharacter::AlignForwardToCamera()
{
	Forward = CameraComponent->GetForwardVector();
	Right = CameraComponent->GetRightVector();
}

void ASokobanCharacter::RotateCamera(const FInputActionValue& Value)
{
	float Direction = Value.Get<float>();

	if (SpringArmComponent)
	{
		// Get the current rotation of the camera
		FRotator CurrentRotation = SpringArmComponent->GetRelativeRotation();

		if (Direction >= 0.f)
		{
			CurrentRotation.Yaw += 90.0f;
		}
		else
		{
			CurrentRotation.Yaw -= 90.0f;
		}

		// Set the new rotation to the camera component
		SpringArmComponent->SetRelativeRotation(CurrentRotation);
		AlignForwardToCamera();
	}
}

void ASokobanCharacter::SetInitialMappingContext()
{
	SokobanPlayerController = Cast<APlayerController>(GetController());

	if (SokobanPlayerController)
	{
		//In Order for this to work, had to add "EnhancedInput" Module in ToonTanks.Build.cs Script
		//And then include "EnhancedInputSubsystems.h", Close Editor, Build, Open Editor, Tools->Refresh VS Project
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(SokobanPlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
}

void ASokobanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Casting Old UInputComponent to New UEnhancedInputComponent N: #include "EnhancedInputComponent.h"
	//CastChecked Assesses that the cast has been successful, if not, crashes intentionally
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//N: #include "Components/InputComponent.h"
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASokobanCharacter::HandleInput);
		EnhancedInputComponent->BindAction(QuitGameAction, ETriggerEvent::Triggered, this, &ASokobanCharacter::Quit);
		EnhancedInputComponent->BindAction(ResetAction, ETriggerEvent::Triggered, this, &ASokobanCharacter::Reset);
		EnhancedInputComponent->BindAction(RotateCameraAction, ETriggerEvent::Triggered, this, &ASokobanCharacter::RotateCamera);
	}

}



