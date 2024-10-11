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

	if (Character && CanPush())
	{
		if(!bIsMoving)
		{
			Character->SetEnabledMovement(false);
			//Seemingly, attachment rules must be defined
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, true);
			Character->AttachToActor(this, AttachmentRules);
			PushDirection = OtherActorDirection;
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
		FVector Start = Character->GetActorLocation();
		FVector End = Start + Character->GetActorForwardVector() * 100.f;
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(Character);

		bool bHit = World->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);

		//Line Trace from Character Hit this Block
		if (bHit && HitResult.GetActor() == this)
		{
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

