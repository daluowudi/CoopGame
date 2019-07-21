// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ASGameMode();

	void StartPlay() override;
protected:
	FTimerHandle TimerHandle_BotSpawn;

	int32 NumOfBotsToSpawn;

	int32 CurrentWave;
	
	UPROPERTY(EditDefaultsOnly, Category="SpawnEnemy")
	float TimeBetWeenWaves;
protected:
	UFUNCTION(BlueprintImplementableEvent, Category="GameMode")
	void SpawnNewBot();

	void StartWave();

	void EndWave();

	void PrepareForNextWave();

	void SpawnBotTick();
};
