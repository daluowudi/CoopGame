// Fill out your copyright notice in the Description page of Project Settings.

#include "SubmachineGun.h"
#include "CoopGame.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Net/UnrealNetwork.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Debug Lines for Weapons"),
	ECVF_Cheat);
	
ASubmachineGun::ASubmachineGun()
{
	TraceSocketName = "BeamEnd";

	SpreadConeDegrees = 0.0f;
}

void ASubmachineGun::Fire()
{
	Super::Fire();

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

		float RandRadians = FMath::DegreesToRadians(SpreadConeDegrees/2);

		ShootDirection = FMath::VRandCone(ShootDirection, RandRadians);

		FVector EndPoint = StartPoint + ShootDirection * 10000.0;

		FVector TraceEnd = EndPoint;

		EPhysicalSurface HitSurfaceType = SurfaceType_Default;
		bool bIsHitedSomething = false;

		if (GetWorld()->LineTraceSingleByChannel(Hit, StartPoint, EndPoint, ECC_WEAPON, QueryParams))
		{
            bIsHitedSomething = true;
			HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float HitDamage = HitSurfaceType == SURFACE_FLESHVULNERABLE ? BaseDamage * HeadShootRatio : BaseDamage;

			UGameplayStatics::ApplyPointDamage(Hit.GetActor(), HitDamage, ShootDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);

			TraceEnd = Hit.ImpactPoint;

			ApplyImpulseEffect(bIsHitedSomething, TraceEnd, HitSurfaceType);
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

		ApplyMuzzleEffect();
		ApplyTraceEffect(TraceEnd);

		if (HasAuthority())
		{
	       	HitScanTrace.TraceTo = TraceEnd;
	       	HitScanTrace.SurfaceType = HitSurfaceType;
	       	HitScanTrace.bHitTarget = bIsHitedSomething;
		}
	}
}

void ASubmachineGun::ApplyTraceEffect(FVector TraceEnd)
{
	// 轨迹特效
	if (TraceEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* ParticleComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceEffect, MuzzleLocation);
		ParticleComponent->SetVectorParameter(TraceSocketName, TraceEnd);
	}
}

void ASubmachineGun::ApplyImpulseEffect(bool bIsHit, FVector EndPoint, EPhysicalSurface HitSurfaceType)
{
	/* 
		不管用，如果加上这层判断的话，server射击时client无法看到受击效果
		有点疑惑，为什么不能传过来，之后测试一下别的值
	*/
	// if (bIsHit)
	// {	
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
			FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
			FVector ShootDirection = EndPoint - MuzzleLocation;
	        ShootDirection.Normalize();

			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedImpactEffect, EndPoint, ShootDirection.Rotation());
        }
	// }
}

void ASubmachineGun::OnRep_HitScanTrace()
{
	ApplyMuzzleEffect();
	ApplyTraceEffect(HitScanTrace.TraceTo);
	ApplyImpulseEffect(HitScanTrace.bHitTarget, HitScanTrace.TraceTo, HitScanTrace.SurfaceType);
}

void ASubmachineGun::GetLifetimeReplicatedProps( TArray< class FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASubmachineGun, HitScanTrace, COND_SkipOwner);
}
