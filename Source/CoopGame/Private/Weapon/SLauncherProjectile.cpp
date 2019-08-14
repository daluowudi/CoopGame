// Fill out your copyright notice in the Description page of Project Settings.

#include "SLauncherProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ASLauncherProjectile::ASLauncherProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	GravityScale = 1.0;
	BoomDelay = 1.0;

	ExplodeDamage = 50.0f;
	ExplodeRadius = 50.0f;


	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	MovementComp->UpdatedComponent = RootComponent;
	MovementComp->InitialSpeed = 0.0f;
	MovementComp->Velocity = FVector::ZeroVector;
	MovementComp->MaxSpeed = 1000.0f;
	MovementComp->bRotationFollowsVelocity = false;
	MovementComp->bShouldBounce = true;
	MovementComp->SetActive(false);

	SetReplicates(true);
	SetReplicateMovement(true);
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

void ASLauncherProjectile::Launch(FVector Velocity, AActor* Causer)
{
	if (MovementComp)
	{
		MovementComp->Velocity = Velocity;
		MovementComp->SetActive(true);
	}

	DamageCauser = Causer;

	GetWorld()->GetTimerManager().ClearTimer(BoomTimerHandler);
	GetWorld()->GetTimerManager().SetTimer(BoomTimerHandler, this, &ASLauncherProjectile::onProjectileExplode, BoomDelay, false, -1);
}

void ASLauncherProjectile::OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	// UE_LOG(LogTemp, Log, TEXT("Now ASLauncherProjectile Bounces"));
}

void ASLauncherProjectile::onProjectileExplode()
{
	MeshComp->SetVisibility(false, false);

	UGameplayStatics::SpawnEmitterAtLocation(this, ExplodeEffect, GetActorLocation());
	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation(), FRotator::ZeroRotator);

	SetLifeSpan(1.0);

	if (HasAuthority())
	{
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(this);

		UGameplayStatics::ApplyRadialDamage(this, ExplodeDamage, GetActorLocation(), ExplodeRadius, nullptr, IgnoreActors, DamageCauser, nullptr, true);

		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), GetActorLocation(), ExplodeRadius, 8, FColor::Red, 5.0f);
	}
}
// Called every frame
// void ASLauncherProjectile::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);

// }
