// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SokobanCharacter.h"

// Sets default values
ABlock::ABlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABlock::BeginPlay()
{
	Super::BeginPlay();

	//N: FindComponentByClass CANNOT be called before BeginPlay
	BoxComponent = FindComponentByClass<UBoxComponent>();
	if (BoxComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("BoxComponent not set!"));
	}
	Character = Cast<ASokobanCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
}

// Called every frame
void ABlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMoving)
	{
		Move(DeltaTime);
		return;
	}
	if (bIsFalling)
	{
		Fall(DeltaTime);
	}
}

bool ABlock::CanBePushed(FVector Direction)
{
	UWorld* World = GetWorld();
	if (World)
	{
		PushDirection = Direction;

		//Box Trace from this Block did not find an obstacle in pushing direction
		FVector Start = GetActorLocation();
		//Get World Size of Box, returns half-size EX: 100cm box returns 50cm
		FVector HalfSize = BoxComponent->Bounds.BoxExtent;
		FVector End = Start + PushDirection * PushDistance;
		FQuat BoxRotation = FQuat::Identity;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);
		FHitResult HitResult;

		//Height Offset to not collide with floor (50.f from box half-size + 10.f real offset)
		Start.Z += 60.f;
		End.Z += 60.f;

		bool bObstacleHit = World->SweepSingleByChannel(
			HitResult,
			Start,
			End,
			BoxRotation,
			ECollisionChannel::ECC_Visibility,
			FCollisionShape::MakeBox(HalfSize),
			CollisionParams);

#if WITH_EDITOR
		//Draw Box Path
		DrawDebugBox(World, Start, HalfSize, FColor::Green, false, 2.0f);
		DrawDebugBox(World, End, HalfSize, FColor::Blue, false, 2.0f);
#endif
		//Line Trace to check for edges on map
		Start = End;
		End.Z -= 200.f;
		FHitResult EdgeHitResult;

		bool bFloorHit = World->LineTraceSingleByChannel(EdgeHitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionParams);

		if (EdgeHitResult.Distance >= 150.f)
		{
			bFloorHit = true;
			bBeyondEdge = true;
		}

		if (!bObstacleHit && bFloorHit)
		{
			return true;
		}
	}
	return false;
}

void ABlock::PushBlock()
{
	TargetLocation = GetActorLocation() + PushDirection * PushDistance;
	bIsMoving = true;

	//Play Sound
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), PushSound, GetActorLocation());
}

void ABlock::Move(float DeltaTime)
{
	FVector CurrentLocation = GetActorLocation();

	float MovementSpeed = (PushDirection * PushDistance).Length() / PushTime;

	//Move
	FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, MovementSpeed);
	SetActorLocation(NewLocation);

	//Stop moving
	if (FVector::Dist(CurrentLocation, TargetLocation) < KINDA_SMALL_NUMBER)
	{
		SetActorLocation(TargetLocation);
		bIsMoving = false;
		//Character->SetEnabledMovement(true);
		//Character->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		if (bBeyondEdge)
		{
			bBeyondEdge = false;
			bIsFalling = true;
			TargetLocation = GetActorLocation() + GetActorUpVector() * -100.f;
		}
	}
}

void ABlock::Fall(float DeltaTime)
{
	FVector CurrentLocation = GetActorLocation();
	float MovementSpeed = (GetActorUpVector() * -100.f).Length() / 0.2f;

	//Move
	FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, MovementSpeed);
	SetActorLocation(NewLocation);

	//Stop moving
	if (FVector::Dist(CurrentLocation, TargetLocation) < KINDA_SMALL_NUMBER)
	{
		SetActorLocation(TargetLocation);
		bIsFalling = false;
	}
}

