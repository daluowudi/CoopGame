// Fill out your copyright notice in the Description page of Project Settings.

#include "SPowerupActor.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Net/UnrealNetwork.h"

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

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASPowerupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASPowerupActor::OnRep_Activate()
{
	if (bIsActivated)
	{
		// 隐藏自身
		MeshComp->SetVisibility(false, true);
	}
	else
	{
		// 先什么都不需要做
	}
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
	OnRep_Activate();

	if (PowerupTickInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandler, this, &ASPowerupActor::OnTickPowerup, PowerupTickInterval, true);
	}
	else
	{
		OnTickPowerup();
	}

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
		OnRep_Activate();

		TargetPawn = nullptr;

		SetLifeSpan(1.0f);
	}
}

void ASPowerupActor::GetLifetimeReplicatedProps( TArray< class FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASPowerupActor, bIsActivated);
}