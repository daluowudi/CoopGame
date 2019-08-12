// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/SWeaponBase.h"
#include "GrenadeLauncher.generated.h"

class ASLauncherProjectile;

UCLASS()
class COOPGAME_API AGrenadeLauncher : public ASWeaponBase
{
	GENERATED_BODY()

public:

	AGrenadeLauncher();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<ASLauncherProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float LaunchVelocity;

	virtual void Fire() override;
};
