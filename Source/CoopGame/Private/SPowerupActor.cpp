// Fill out your copyright notice in the Description page of Project Settings.

#include "SPowerupActor.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ASPowerupActor::ASPowerupActor()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;

	bIsActivated = false;

	PowerupTickInterval = 0.0f;
	PowerupTickTimes = 0;
}

// Called when the game starts or when spawned
void ASPowerupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASPowerupActor::DoActivate(AActor* ActivateFor)
{
	if (bIsActivated)
	{
		return;
	}

	bIsActivated = true;

	if (PowerupTickInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandler, this, &ASPowerupActor::OnTickPowerup, PowerupTickInterval, true);
	}
	else
	{
		OnTickPowerup();
	}

	// 隐藏自身
	MeshComp->SetVisibility(false, false);

	OnActivated();
}

void ASPowerupActor::OnTickPowerup()
{
	CurrentTickTimes++;

	OnPowerupTicked();

	if (CurrentTickTimes >= PowerupTickTimes)
	{
		GetWorldTimerManager().ClearTimer(TimerHandler);

		OnExpired();

		bIsActivated = false;

		SetLifeSpan(1.0f);
	}
}