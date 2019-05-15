// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"
#include "CoopGame.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraShake.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

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
		QueryParams.bReturnPhysicalMaterial = true;

		MyOwner->GetActorEyesViewPoint(StartPoint, Rotation);
		
		FVector ShootDirection = Rotation.Vector();
		FVector EndPoint = StartPoint + ShootDirection * 10000.0;

		FVector TraceEnd = EndPoint;

		if (GetWorld()->LineTraceSingleByChannel(Hit, StartPoint, EndPoint, ECC_Visibility, QueryParams))
		{
			UGameplayStatics::ApplyPointDamage(Hit.GetActor(), 20.0, ShootDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);

			TraceEnd = Hit.ImpactPoint;

			EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
			UParticleSystem* SelectedImpactEffect = nullptr;
			switch(HitSurfaceType){
				case SURFACE_FLESHDEFAULT:
				case SURFACE_FLESHVULNERABLE:
					SelectedImpactEffect = FleshImpactEffect;
					break;
				default:
					SelectedImpactEffect = DefaultImpactEffect;
					break;

			}

			if (SelectedImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedImpactEffect, Hit.ImpactPoint);				
			}
		}

		// UCameraShake
		APawn* MyPawn = Cast<APawn>(MyOwner);
		if (MyPawn)
		{
			APlayerController* PC = Cast<APlayerController>(MyPawn->GetController());	
			if (PC)
			{
				PC->ClientPlayCameraShake(CameraShake);
			}
		}
		
		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::White, false, 1, 0, 2);
		}

		ApplyEffect(TraceEnd);
	}
}

void ASWeapon::ApplyEffect(FVector TraceEnd)
{	
	// 枪口特效
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}
	// 轨迹特效
	if (TraceEffect)
	{
		FVector TraceLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* ParticleComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceEffect, TraceLocation);
		ParticleComponent->SetVectorParameter(TraceSocketName, TraceEnd);
	}
}