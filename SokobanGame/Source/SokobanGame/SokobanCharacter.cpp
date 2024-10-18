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
			bIsPushing = true;
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

void ASokobanCharacter::Movement(float DeltaTime)
{
	FVector CurrentLocation = GetActorLocation();
	/*TargetLocation = CurrentLocation + PushDirection * PushDistance;*/
	float MovementSpeed = (CurrentDirection * MoveDistance).Length() / MoveTime;

	//Move
	FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, MovementSpeed);
	SetActorLocation(NewLocation);

	//Stop moving
	if (FVector::Dist(CurrentLocation, TargetLocation) < 10)
	{
		SetActorLocation(TargetLocation);
		bIsMoving = false;
		SetEnabledMovement(true);
	}

	// Calculate the target rotation using the direction vector
	FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(CurrentDirection);

	// Smoothly interpolate the current rotation towards the target rotation
	FRotator CurrentRotation = GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 10.0f); // 10.0f is the rotation speed

	// Apply the new rotation to the character
	SetActorRotation(NewRotation);
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

// Called to bind functionality to input
void ASokobanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Casting Old UInputComponent to New UEnhancedInputComponent N: #include "EnhancedInputComponent.h"
	//CastChecked Assesses that the cast has been successful, if not, crashes intentionally
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//N: #include "Components/InputComponent.h"
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASokobanCharacter::Move);
		EnhancedInputComponent->BindAction(QuitGameAction, ETriggerEvent::Triggered, this, &ASokobanCharacter::Quit);
		EnhancedInputComponent->BindAction(ResetAction, ETriggerEvent::Triggered, this, &ASokobanCharacter::Reset);
		EnhancedInputComponent->BindAction(RotateCameraAction, ETriggerEvent::Triggered, this, &ASokobanCharacter::RotateCamera);
	}

}

bool ASokobanCharacter::IsValidMove(FVector Direction)
{

	//Check for edges

	UWorld* World = GetWorld();
	if (World)
	{
		//Direction.Normalize();
		//Line Trace to check for edges on map
		FVector Start = GetActorLocation();
		FVector End = Start + Direction * MoveDistance;
		FHitResult HitResult;

		bool bHit = World->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);
		DrawDebugLine(World, Start, End, FColor::Black, false, 2.0f);
		if (bHit)
		{
			AActor* HitActor = HitResult.GetActor();
			if (HitActor && HitActor->IsA(ABlock::StaticClass()))
			{
				//UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *HitActor->GetName());

				ABlock* BlockToMove = Cast<ABlock>(HitActor);
				if (!BlockToMove->HasObstacle(Direction))
				{
					BlockToMove->PushBlock();
					bIsPushing = true;
				}
				else
				{
					return false;
				}
				return true;
			}

			DrawDebugLine(World, Start, End, FColor::Red, false, 2.0f);
			//UE_LOG(LogTemp, Warning, TEXT("Forward hit: %s"), *HitResult.GetActor()->GetName());
			return false;
		}
		

	}
	//UE_LOG(LogTemp, Warning, TEXT("Valid Move!"));
	return true;
}

void ASokobanCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (bIsMoving || bIsPushing)
	{
		return;
	}

	if (MovementVector.X != 0.f && MovementVector.Y != 0.f)
	{
		return;
	}

	if (MovementVector.IsNearlyZero())
	{
		return;
	}

	//Align forward with camera's forward, Should not be here
	Forward = CameraComponent->GetForwardVector();
	Right = CameraComponent->GetRightVector();

	MovementVector.X = FMath::Sign(MovementVector.X);
	MovementVector.Y = FMath::Sign(MovementVector.Y);

	//Scale movement using input vector
	FVector WorldDirection = (Forward * MovementVector.Y) + (Right * MovementVector.X);
	WorldDirection.Z = 0.f;
	WorldDirection.Normalize();

	//UE_LOG(LogTemp, Warning, TEXT("World Direction: %s"), *WorldDirection.ToString());

	if (!EdgeInDirection(WorldDirection) && IsValidMove(WorldDirection))
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
	}
}

bool ASokobanCharacter::EdgeInDirection(FVector Direction)
{
	UWorld* World = GetWorld();
	if (World)
	{
		//Direction.Normalize();
		//Line Trace to check for edges on map
		//FVector Start = GetActorLocation() + GetActorForwardVector() * 100.f;
		FVector Start = GetActorLocation() + Direction * EdgeOffsetDistance;
		FVector End = Start - FVector(0.f,0.f, EdgeDepthCheckDistance);
		FHitResult EdgeHitResult;

		bool bFloorHit = World->LineTraceSingleByChannel(EdgeHitResult, Start, End, ECollisionChannel::ECC_Visibility);

		DrawDebugLine(World, Start, End, FColor::Blue, false, 1.0f);
		if (bFloorHit)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Floor hit: %s"), *EdgeHitResult.GetActor()->GetName());
			return false;
		}

	}
	UE_LOG(LogTemp, Warning, TEXT("No Floor hit!"));
	return true;
}

