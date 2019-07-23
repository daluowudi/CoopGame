// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameMode.h"
#include "TimerManager.h"
#include "SHealthComponent.h"


ASGameMode::ASGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

	CurrentWave = 0;
	TimeBetWeenWaves = 2.0f;
}

void ASGameMode::StartPlay()
{
	Super::StartPlay();

	StartWave();
}

void ASGameMode::StartWave()
{
	CurrentWave++;
	NumOfBotsToSpawn = CurrentWave * 2;

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawn, this, &ASGameMode::SpawnBotTick, 1.0f, true, 0.0f);
}

void ASGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawn);
	TimerHandle_BotSpawn.Invalidate();

	// PrepareForNextWave();
}

void ASGameMode::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_WaveHandle, this, &ASGameMode::StartWave, TimeBetWeenWaves, false);
}

void ASGameMode::SpawnBotTick()
{
	NumOfBotsToSpawn--;
	SpawnNewBot();

	if (NumOfBotsToSpawn == 0)
	{	
		EndWave();
	}
}

void ASGameMode::CheckWaveState()
{
	if (NumOfBotsToSpawn > 0.0f)
	{
		return;
	}

	if (GetWorldTimerManager().IsTimerActive(TimerHandle_WaveHandle))
	{
		return;
	}

	bool bBotAllKilled = true;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		const APawn* ItPawn = It->Get();
		if (IsValid(ItPawn) && !ItPawn->IsPlayerControlled())
		{
			USHealthComponent* HealthComponent = Cast<USHealthComponent>(ItPawn->GetComponentByClass(USHealthComponent::StaticClass()));

			if (HealthComponent && HealthComponent->GetHealth() > 0.0f)
			{
				bBotAllKilled = false;
				break;
			}
		}
	}

	if (bBotAllKilled)
	{
		PrepareForNextWave();
	}
}

void ASGameMode::CheckAnyPlayerAlive()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Iterator->Get();
		if (IsValid(PC) && IsValid(PC->GetPawn()))
		{
			APawn* Pawn = PC->GetPawn();
			USHealthComponent* HealthComponent = Cast<USHealthComponent>(Pawn->GetComponentByClass(USHealthComponent::StaticClass()));

			if (HealthComponent && HealthComponent->GetHealth() > 0.0f)
			{
				return;
			}
		}
	}

	GameOver();
}

void ASGameMode::GameOver()
{
	EndWave();
}

void ASGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckWaveState();
	CheckAnyPlayerAlive();
}