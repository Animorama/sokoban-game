// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxTrigger.h"
#include "DrawDebugHelpers.h"
#include "Block.h"
#include "SokobanGameMode.h"

ABoxTrigger::ABoxTrigger()
{
	OnActorBeginOverlap.AddDynamic(this, &ABoxTrigger::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &ABoxTrigger::OnOverlapEnd);

}

void ABoxTrigger::BeginPlay()
{
	Super::BeginPlay();

	GameMode = GetWorld()->GetAuthGameMode<ASokobanGameMode>();

	DrawDebugBox(GetWorld(), GetActorLocation(), GetComponentsBoundingBox().GetExtent(), FColor::Green, true, -1, 0, 5);
}

void ABoxTrigger::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{

	if (OtherActor && (this != OtherActor) && OtherActor->IsA(ABlock::StaticClass()))
	{
		//UE_LOG(LogTemp, Warning, TEXT("%s was overlapped"), *OverlappedActor->GetName());
		//UE_LOG(LogTemp, Warning, TEXT("%s has entered the box"), *OtherActor->GetName());
		bIsOccupied = true;
		if (GameMode != nullptr)
		{
			GameMode->BlockPlaced(OtherActor, OverlappedActor);
		}
	}
}

void ABoxTrigger::OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor && (this != OtherActor) && OtherActor->IsA(ABlock::StaticClass()))
	{
		//UE_LOG(LogTemp, Warning, TEXT("%s has entered the box"), *OtherActor->GetName());
		bIsOccupied = false;
	}
}

