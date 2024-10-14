// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"
#include "Components/StaticMeshComponent.h"
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
	BlockMeshComponent = FindComponentByClass<UStaticMeshComponent>();
	if (BlockMeshComponent != nullptr)
	{
		//Function used to bind our callback function to invocation list of OnComponentHit Event
		BlockMeshComponent->OnComponentHit.AddDynamic(this, &ABlock::OnHit);
	}
	Character = Cast<ASokobanCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
}

// Called every frame
void ABlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMoving)
	{
		MoveBlock(DeltaTime);
	}
}

void ABlock::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{	
	float VectorX = (FMath::Abs(OtherActor->GetActorForwardVector().X) >= 0.5f) ? FMath::Sign(OtherActor->GetActorForwardVector().X) : 0.f;
	float VectorY = (FMath::Abs(OtherActor->GetActorForwardVector().Y) >= 0.5f) ? FMath::Sign(OtherActor->GetActorForwardVector().Y) : 0.f;
	FVector OtherActorDirection(VectorX, VectorY, 0.f);
	//UE_LOG(LogTemp, Warning, TEXT("Other Actor Direction: %s"), *OtherActorDirection.ToString());

	PushDirection = OtherActorDirection;

	if (Character && CanPush())
	{
		if(!bIsMoving && !HasObstacle())
		{
			Character->SetEnabledMovement(false);
			//Seemingly, attachment rules must be defined
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, true);
			Character->AttachToActor(this, AttachmentRules);
			TargetLocation = GetActorLocation() + PushDirection * PushDistance;
			bIsMoving = true;
		}
	}
}

bool ABlock::CanPush()
{
	UWorld* World = GetWorld();
	if (Character && World)
	{
		//Line Trace from Character Hit this Block
		FVector Start = Character->GetActorLocation();
		FVector End = Start + Character->GetActorForwardVector() * 100.f;
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(Character);

		bool bHit = World->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);
		
		//Make Sure Block only moves in cardinal directions
		float Dot = static_cast<float>(HitResult.Normal.Dot(Character->GetActorForwardVector()));
		
		if (bHit && HitResult.GetActor() == this && FMath::Abs(Dot) >= 0.92f)
		{
			return true;
		}

	}
	return false;
}

bool ABlock::HasObstacle()
{
	UWorld* World = GetWorld();
	if (World)
	{
		//Box Trace from this Block did not find an obstacle in pushing direction
		FVector Start = GetActorLocation();
		//Get World Size of Box, returns half-size EX: 100cm box returns 50cm
		FVector HalfSize = BlockMeshComponent->Bounds.BoxExtent;
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

		//Draw Box Path
		DrawDebugBox(World, Start, HalfSize, FColor::Green, false, 2.0f);
		DrawDebugBox(World, End, HalfSize, FColor::Blue, false, 2.0f);

		//Line Trace to check for edges on map
		Start = End;
		End.Z -= 100.f;
		FHitResult EdgeHitResult;

		bool bFloorHit = World->LineTraceSingleByChannel(EdgeHitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionParams);

		DrawDebugLine(World, Start, End, FColor::Red, false, 2.0f);

		if (bObstacleHit || !bFloorHit)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Actor Hit: %s"), *HitResult.GetActor()->GetName());
			return true;
		}
	}
	return false;
}

void ABlock::MoveBlock(float DeltaTime)
{
	FVector CurrentLocation = GetActorLocation();
	/*TargetLocation = CurrentLocation + PushDirection * PushDistance;*/
	float MovementSpeed = (PushDirection * PushDistance).Length() / PushTime;

	//Move
	FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, MovementSpeed);
	SetActorLocation(NewLocation);

	//Stop moving
	if (FVector::Dist(CurrentLocation, TargetLocation) < KINDA_SMALL_NUMBER)
	{
		SetActorLocation(TargetLocation);
		bIsMoving = false;
		Character->SetEnabledMovement(true);
		Character->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}

