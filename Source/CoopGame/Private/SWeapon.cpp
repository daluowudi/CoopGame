// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Debug Lines for Weapons"),
	ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TraceSocketName = "BeamEnd";
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASWeapon::Fire()
{
	AActor* MyOwner = GetOwner();

	if (MyOwner)
	{
		FHitResult Hit;
		FVector StartPoint;
		FRotator Rotation;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredComponent(MeshComp);
		QueryParams.bTraceComplex = true;
		// QueryParams.bReturnPhysicalMaterial = true;

		MyOwner->GetActorEyesViewPoint(StartPoint, Rotation);
		
		FVector ShootDirection = Rotation.Vector();
		FVector EndPoint = StartPoint + ShootDirection * 10000.0;

		FVector TraceEnd = EndPoint;

		if (GetWorld()->LineTraceSingleByChannel(Hit, StartPoint, EndPoint, ECC_Visibility, QueryParams))
		{
			UGameplayStatics::ApplyPointDamage(Hit.GetActor(), 20.0, ShootDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);

			TraceEnd = Hit.ImpactPoint;

			if (ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint);				
			}
		}

		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::White, false, 1, 0, 2);
		}		

		if (MuzzleEffect)
		{
			UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
		}

		if (TraceEffect)
		{
			FVector TraceLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

			UParticleSystemComponent* ParticleComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceEffect, TraceLocation);
			ParticleComponent->SetVectorParameter(TraceSocketName, TraceEnd);
		}
	}
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

