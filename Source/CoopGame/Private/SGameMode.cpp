// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameMode.h"
#include "TimerManager.h"

ASGameMode::ASGameMode()
{
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
	PrepareForNextWave();
}

void ASGameMode::EndWave()
{
	FTimerHandle TimerHandle_WaveHandle;

	GetWorldTimerManager().SetTimer(TimerHandle_WaveHandle, this, &ASGameMode::PrepareForNextWave, TimeBetWeenWaves, false);
}

void ASGameMode::PrepareForNextWave()
{
	CurrentWave++;
	NumOfBotsToSpawn = CurrentWave * 2;

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawn, this, &ASGameMode::SpawnBotTick, 1.0f, true, 0.0f);
}

void ASGameMode::SpawnBotTick()
{
	NumOfBotsToSpawn--;
	SpawnNewBot();

	if (NumOfBotsToSpawn == 0)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawn);
		TimerHandle_BotSpawn.Invalidate();
		
		EndWave();
	}
}
