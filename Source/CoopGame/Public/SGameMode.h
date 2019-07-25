// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilledSignature, AActor*, DeadActor, AActor*, DamageCauser, class AController*, InstigatedBy);

enum class EWaveState : uint8;

/**
 * 
 */
UCLASS()
class COOPGAME_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnActorKilledSignature OnActorKilled;
public:
	ASGameMode();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaTime) override;

	// virtual void RestartPlayer(AController* NewPlayer) override;
protected:
	FTimerHandle TimerHandle_BotSpawn;
	FTimerHandle TimerHandle_WaveHandle;

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

	void CheckWaveState();

	void CheckAnyPlayerAlive();

	void GameOver();

	void SetWaveState(EWaveState NewState);

	void RestartDeadPlayers();
};
