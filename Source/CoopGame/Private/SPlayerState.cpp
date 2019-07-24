// Fill out your copyright notice in the Description page of Project Settings.

#include "SPlayerState.h"
#include "SGameMode.h"

ASPlayerState::ASPlayerState()
{
	KillScore = 20.0f;
}

void ASPlayerState::BeginPlay()
{
	Super::BeginPlay();

	ASGameMode* GS = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
	if (GS)
	{
		GS->OnActorKilled.AddDynamic(this, &ASPlayerState::OnActorKilled);
	}
}

void ASPlayerState::OnActorKilled(AActor* DeadActor, AActor* DamageCauser, class AController* InstigatedBy)
{
	// 玩家死亡不加分
	APawn* DeadPawn = Cast<APawn>(DeadActor);
	if (DeadPawn && DeadPawn->IsPlayerControlled())
	{
		return;
	}
	
	AddScore(KillScore);		
}

void ASPlayerState::AddScore(float DeltaScore)
{
	Score += DeltaScore;

	// UE_Log(LogTemp, Log, TEXT("AddScore"))
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Add Score!"));
}