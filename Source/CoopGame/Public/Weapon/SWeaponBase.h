// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeaponBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAmmoChangedSignature, class AActor*, Owner, class ASWeaponBase*, Weapon, int32, AmmoNum);

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class UCameraShake;

UCLASS()
class COOPGAME_API ASWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effect")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<class UCameraShake> CameraShake;

	/* shoot per minute */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = 1, ClampMax = 9999))
	float ShootRate;

	float TimeBetweenShoot;

	float LastShootTime;

	// 最大弹药量
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = 1, ClampMax = 500))
	float AmmoPerClip;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentAmmo, BlueprintReadOnly, Category = "Weapon")
	int32 CurrentAmmo;

	UFUNCTION()
	void OnRep_CurrentAmmo();

	FTimerHandle TimerHandle_AutomaticFire;

	void ApplyMuzzleEffect();

	virtual void Fire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	UFUNCTION()
	void OnOwnerDestory(AActor* DiedActor);
public:	

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAmmoChangedSignature OnAmmoChanged;

	void StartFire();
	void StopFire();

	int32 GetCurrentAmmo() const {return CurrentAmmo;};
};
