// Fill out your copyright notice in the Description page of Project Settings.

#include "SPickupActor.h"
#include "SPowerupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"

// Sets default values
ASPickupActor::ASPickupActor()
{
	OverlapComp = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapComp"));
	RootComponent = OverlapComp;

	OverlapComp->SetSphereRadius(75.0f);
	OverlapComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	DecalComp->DecalSize = FVector(64, 75, 75);
	DecalComp->SetupAttachment(OverlapComp);

	CoolDownDuration = 5.0f;
	PowerupActorHeight = 0.0f;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();

	PawnPowerupActor();
}

void ASPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!PowerupInstance)
	{
		return;
	}

	if (!HasAuthority())
	{
		return;
	}

	PowerupInstance->DoActivate(OtherActor);
	PowerupInstance = nullptr;
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandler, this, &ASPickupActor::OnCoolDown, CoolDownDuration);
}

void ASPickupActor::OnCoolDown()
{
	// 已有则不再产生
	if (PowerupInstance)
	{
		return;
	}

	PawnPowerupActor();
}

void ASPickupActor::PawnPowerupActor()
{
	if (!HasAuthority())
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector location = GetActorLocation();
	location.Z += PowerupActorHeight;

	PowerupInstance = GetWorld()->SpawnActor<ASPowerupActor>(*PowerupClass, location, FRotator::ZeroRotator);

	if (!PowerupInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("ASPickupActor::PawnPowerupActor fail to pawn actor"));
	}
}