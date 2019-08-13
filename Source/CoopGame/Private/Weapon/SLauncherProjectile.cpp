// Fill out your copyright notice in the Description page of Project Settings.

#include "SLauncherProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ASLauncherProjectile::ASLauncherProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	GravityScale = 1.0;

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	MovementComp->UpdatedComponent = RootComponent;
	MovementComp->InitialSpeed = 0.0f;
	MovementComp->Velocity = FVector::ZeroVector;
	MovementComp->MaxSpeed = 1000.0f;
	MovementComp->bRotationFollowsVelocity = false;
	MovementComp->bShouldBounce = true;
	MovementComp->SetActive(false);
}

// Called when the game starts or when spawned
void ASLauncherProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (MovementComp)
	{
		MovementComp->ProjectileGravityScale = GravityScale;
		
		MovementComp->OnProjectileBounce.AddDynamic(this, &ASLauncherProjectile::OnProjectileBounce);
	}
}

void ASLauncherProjectile::Launch(FVector Velocity)
{
	if (MovementComp)
	{
		MovementComp->Velocity = Velocity;
		MovementComp->SetActive(true);
	}
}

void ASLauncherProjectile::OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	UE_LOG(LogTemp, Log, TEXT("Now ASLauncherProjectile Bounces"));
}

// Called every frame
// void ASLauncherProjectile::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);

// }
