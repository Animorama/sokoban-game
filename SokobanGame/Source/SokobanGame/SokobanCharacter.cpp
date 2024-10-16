// Fill out your copyright notice in the Description page of Project Settings.


#include "SokobanCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "SokobanPlayerController.h"

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
}

// Called every frame
void ASokobanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	}

}

void ASokobanCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (MovementVector.IsNearlyZero())
	{
		return;
	}

	FVector Direction = FVector(MovementVector.Y, MovementVector.X, 0.f);

	if (!EdgeInDirection(Direction))
	{
		AddMovementInput(Forward, MovementVector.Y);
		AddMovementInput(Right, MovementVector.X);
	}

	////Draw Debug Line Gizmo Of Actor Forward
	//FVector Start = GetActorLocation();
	//FVector End = Start + GetActorForwardVector() * 100.f;
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

bool ASokobanCharacter::EdgeInDirection(FVector Direction)
{
	UWorld* World = GetWorld();
	if (World)
	{
		//Line Trace to check for edges on map
		//FVector Start = GetActorLocation() + GetActorForwardVector() * 100.f;
		FVector Start = GetActorLocation() + Direction * EdgeOffsetDistance;
		FVector End = Start;
		End.Z -= EdgeDepthCheckDistance;
		FHitResult EdgeHitResult;

		bool bFloorHit = World->LineTraceSingleByChannel(EdgeHitResult, Start, End, ECollisionChannel::ECC_Visibility);

		//DrawDebugLine(World, Start, End, FColor::Red, false, 1.0f);
		if (bFloorHit)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Floor hit: %s"), *EdgeHitResult.GetActor()->GetName());
			return false;
		}

	}
	//UE_LOG(LogTemp, Warning, TEXT("No Floor hit!"));
	return true;
}

