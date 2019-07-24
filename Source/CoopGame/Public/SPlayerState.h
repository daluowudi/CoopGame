// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ASPlayerState();
	
	// UFUNCTION(BlueprintCallable, Category="Score")
	void AddScore(float DeltaScore);

	UPROPERTY(EditDefaultsOnly, Category="Score")
	float KillScore;
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnActorKilled(AActor* DeadActor, AActor* DamageCauser, class AController* InstigatedBy);
};
