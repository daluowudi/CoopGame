// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

class UStaticMeshComponent;
class URotatingMovementComponent;

UCLASS()
class COOPGAME_API ASPowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupActor();

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* MeshComp;

	// UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category="Components")
	URotatingMovementComponent* RotatingComp;

	bool bIsActivated;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Powerup")
	float PowerupTickInterval;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Powerup")
	int32 PowerupTickTimes;

	int32 CurrentTickTimes;

	UPROPERTY(BlueprintReadOnly)
	APawn* TargetPawn;
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
