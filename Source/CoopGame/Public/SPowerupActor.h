// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class COOPGAME_API ASPowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupActor();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* MeshComp;

	bool bIsActivated;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Powerup")
	float PowerupTickInterval;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Powerup")
	int32 PowerupTickTimes;

	int32 CurrentTickTimes;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle TimerHandler;
public:	
	void DoActivate(AActor* ActivateFor);

	void OnTickPowerup();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerupFunction")
	void OnActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerupFunction")
	void OnExpired();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "PowerupFunction")
	void OnPowerupTicked();	
};
