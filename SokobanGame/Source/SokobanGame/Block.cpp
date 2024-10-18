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
	BlockMeshComponent = FindComponentByClass<UStaticMeshComponent>();
	BoxComponent = FindComponentByClass<UBoxComponent>();
	if (BoxComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("BoxComponent not set!"));
	}

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
		return;
	}
	if (bIsFalling)
	{
		Fall(DeltaTime);
	}
}

void ABlock::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{	
	//float VectorX = (FMath::Abs(OtherActor->GetActorForwardVector().X) >= 0.5f) ? FMath::Sign(OtherActor->GetActorForwardVector().X) : 0.f;
	//float VectorY = (FMath::Abs(OtherActor->GetActorForwardVector().Y) >= 0.5f) ? FMath::Sign(OtherActor->GetActorForwardVector().Y) : 0.f;
	//FVector OtherActorDirection(VectorX, VectorY, 0.f);
	//UE_LOG(LogTemp, Warning, TEXT("Other Actor: %s"), *OtherActor->GetName());
	////UE_LOG(LogTemp, Warning, TEXT("Other Actor Direction: %s"), *OtherActorDirection.ToString());

	//PushDirection = OtherActorDirection;

	//if (Character && CanBePushed())
	//{
	//	if(!bIsMoving && !HasObstacle())
	//	{
	//		Character->SetEnabledMovement(false);
	//		//Seemingly, attachment rules must be defined
	//		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, true);
	//		Character->AttachToActor(this, AttachmentRules);
	//		TargetLocation = GetActorLocation() + PushDirection * PushDistance;
	//		bIsMoving = true;

	//		//Play sound
	//		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), PushSound, GetActorLocation());
	//	}
	//}
}

bool ABlock::CanBePushed()
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

bool ABlock::HasObstacle(FVector Direction)
{
	UWorld* World = GetWorld();
	if (World)
	{
		PushDirection = Direction;

		//Box Trace from this Block did not find an obstacle in pushing direction
		FVector Start = GetActorLocation();
		//Get World Size of Box, returns half-size EX: 100cm box returns 50cm
		FVector HalfSize = BoxComponent->Bounds.BoxExtent;

		//FVector HalfSize = BlockMeshComponent->Bounds.BoxExtent;
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
		////Debug
		//DrawDebugLine(World, Start, End, FColor::Red, true, 2.0f);
		//AActor* HitActor = EdgeHitResult.GetActor();
		//if (HitActor)
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("Floor Hit: %s"), *HitActor->GetName());
		//	UE_LOG(LogTemp, Warning, TEXT("Floor Hit: %f"), EdgeHitResult.Distance);
		//}

		if (EdgeHitResult.Distance >= 150.f)
		{
			bFloorHit = true;
			bBeyondEdge = true;
		}

		if (bObstacleHit || !bFloorHit)
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

