// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPickupActor.generated.h"

class USphereComponent;
class UDecalComponent;
class ASPowerupActor;

UCLASS()
class COOPGAME_API ASPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPickupActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* OverlapComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDecalComponent* DecalComp;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Powerup")
	TSubclassOf<ASPowerupActor> PowerupClass;

	ASPowerupActor* PowerupInstance;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Powerup")	
	float CoolDownDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Powerup")	
	float PowerupActorHeight;

	FTimerHandle TimerHandler;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void PawnPowerupActor();
public:	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void OnCoolDown();
};
