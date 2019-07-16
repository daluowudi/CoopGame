// Fill out your copyright notice in the Description page of Project Settings.

#include "SPowerupActor.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"

// Sets default values
ASPowerupActor::ASPowerupActor()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;

	RotatingComp = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingComp"));
	RotatingComp->UpdatedComponent = RootComponent;

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

	TargetPawn = Cast<APawn>(ActivateFor);
	// 不是pawn不激活
	if (!TargetPawn)
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

	OnActivated(TargetPawn);
}

void ASPowerupActor::OnTickPowerup()
{
	CurrentTickTimes++;

	OnPowerupTicked(TargetPawn);

	if (CurrentTickTimes >= PowerupTickTimes)
	{
		GetWorldTimerManager().ClearTimer(TimerHandler);

		OnExpired(TargetPawn);

		bIsActivated = false;

		SetLifeSpan(1.0f);
	}
}