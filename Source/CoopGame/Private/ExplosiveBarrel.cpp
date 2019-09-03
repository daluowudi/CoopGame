// Fill out your copyright notice in the Description page of Project Settings.

#include "ExplosiveBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "SHealthComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Materials/MaterialInterface.h"

// Sets default values
AExplosiveBarrel::AExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	// MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	// RadialForceComp->bIgnoreOwningActor = 0;
	// RadialForceComp->ForceStrength = 1000.0;
	RadialForceComp->SetupAttachment(MeshComp);
	RadialForceComp->bIgnoreOwningActor = true;
	RadialForceComp->bAutoActivate = false;
	RadialForceComp->ForceStrength = 1000.0f;
	RadialForceComp->bImpulseVelChange = true;

	bExplode = false;

	ExplodeDamage = 100;
	ExplodeRadius = 100;

	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
	if (HealthComp)
	{
		HealthComp->OnHealthChanged.AddDynamic(this, &AExplosiveBarrel::OnTakeDamage);
	}
}

void AExplosiveBarrel::OnTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0)
	{
		OnExplode();
	}
}

void AExplosiveBarrel::OnExplode()
{
	if (bExplode)
	{
		return;
	}

	bExplode = true;

	UGameplayStatics::SpawnEmitterAtLocation(this, ExplodeEffect, GetActorLocation());

	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation(), FRotator::ZeroRotator);

	// MeshComp->SetVisibility(false, false);

	MeshComp->SetMaterial(0, ExplodedMaterial);

	MeshComp->AddImpulse(FVector::UpVector * 400, NAME_None, true);

	RadialForceComp->FireImpulse();

	if (HasAuthority())
	{
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(this);

		// damage
		UGameplayStatics::ApplyRadialDamage(this, ExplodeDamage, GetActorLocation(), ExplodeRadius, nullptr, IgnoreActors, this);
		// debug sphere
		// UKismetSystemLibrary::DrawDebugSphere(GetWorld(), GetActorLocation(), ExplodeRadius, 8, FColor::Red, 5.0f);

		// SetLifeSpan(2.0f);
	}	
}
// Called every frame
// void AExplosiveBarrel::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);

// }

