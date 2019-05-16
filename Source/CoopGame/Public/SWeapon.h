// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class UCameraShake;

UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

	virtual void BeginPlay() override;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float HeadShootRatio;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	UParticleSystem* TraceEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effect")
	FName MuzzleSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effect")
	FName TraceSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<class UCameraShake> CameraShake;

	/* shoot per minute */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = 1, ClampMax = 9999))
	float ShootRate;

	float TimeBetweenShoot;

	float LastShootTime;

	FTimerHandle TimerHandle_AutomaticFire;
	void ApplyEffect(FVector EndPoint);

	void Fire();
public:	

	void StartFire();
	void StopFire();
};
