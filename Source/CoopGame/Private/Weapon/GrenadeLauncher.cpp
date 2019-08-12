// Fill out your copyright notice in the Description page of Project Settings.

#include "GrenadeLauncher.h"
#include "SLauncherProjectile.h"

AGrenadeLauncher::AGrenadeLauncher()
{
	LaunchVelocity = 1.0f;
}

void AGrenadeLauncher::Fire()
{
	Super::Fire();

	AActor* MyOwner = GetOwner();

	if (MyOwner)
	{
		// 产生子弹
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = MyOwner; // 这里暂时把owner设置为character
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ASLauncherProjectile* Projectile = GetWorld()->SpawnActor<ASLauncherProjectile>(ProjectileClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		// 将子弹发射出去
		FVector StartPoint;
		FRotator Rotation;

		MyOwner->GetActorEyesViewPoint(StartPoint, Rotation);

		FVector ShootDirection = Rotation.Vector();

		Projectile->Launch(ShootDirection * LaunchVelocity);

		// 特效
		ApplyMuzzleEffect();
	}
}