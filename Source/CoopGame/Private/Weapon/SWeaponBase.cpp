// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeaponBase.h"
#include "CoopGame.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraShake.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASWeaponBase::ASWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";

	BaseDamage = 20.0f;
	HeadShootRatio = 3.0f;

	ShootRate = 120.0f;

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 30.0f;

	AmmoPerClip = 30;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	TimeBetweenShoot = 60 / ShootRate; 

	LastShootTime = -TimeBetweenShoot;

	CurrentAmmo = AmmoPerClip;

	if (HasAuthority())
	{
		AActor* MyOwner = GetOwner();

		if (MyOwner)
		{
			MyOwner->OnDestroyed.AddDynamic(this, &ASWeaponBase::OnOwnerDestory);
		}
	}
}

void ASWeaponBase::Fire()
{
	if (!HasAuthority())
	{
		ServerFire();
		// return;
	}
// 对于这种写法始终还是有点想不通，为何不能return呢，由于拆分出了基类不明显了，不拆分出来的话，下面是开火相关内容
	LastShootTime = GetWorld()->TimeSeconds;
	
	CurrentAmmo--;
	OnAmmoChanged.Broadcast(GetOwner(), this, CurrentAmmo);
}

void ASWeaponBase::StartFire()
{
	float FirstDelay = FMath::Max((LastShootTime + TimeBetweenShoot - GetWorld()->TimeSeconds), 0.0f);
	
	GetWorldTimerManager().SetTimer(TimerHandle_AutomaticFire, this, &ASWeaponBase::Fire, TimeBetweenShoot, true, FirstDelay);
}

void ASWeaponBase::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_AutomaticFire);
}

void ASWeaponBase::ApplyMuzzleEffect()
{
	// 枪口特效
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}
}

void ASWeaponBase::ServerFire_Implementation()
{
	Fire();
}

bool ASWeaponBase::ServerFire_Validate()
{
	return true;
}

void ASWeaponBase::OnRep_CurrentAmmo()
{
	OnAmmoChanged.Broadcast(GetOwner(), this, CurrentAmmo);
}

void ASWeaponBase::OnOwnerDestory(AActor* DiedActor)
{
	Destroy();
}

void ASWeaponBase::GetLifetimeReplicatedProps( TArray< class FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(ASWeaponBase, CurrentAmmo, COND_SkipOwner);
}
