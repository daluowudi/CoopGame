// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/SWeaponBase.h"
#include "SubmachineGun.generated.h"

USTRUCT()
struct FHitTrace
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FVector_NetQuantize TraceTo;
	// 是不是枚举类型的都需要声明为 TEnumAsByte<enumname> 呢
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
	bool bHitTarget;
};

UCLASS()
class COOPGAME_API ASubmachineGun : public ASWeaponBase
{
	GENERATED_BODY()	
public:

	ASubmachineGun();
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	UParticleSystem* TraceEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effect")
	FName TraceSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = 0, ClampMax = 60))
	float SpreadConeDegrees;

	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitTrace HitScanTrace;

	void ApplyTraceEffect(FVector TraceEnd);

	UFUNCTION()
	void OnRep_HitScanTrace();

	void ApplyImpulseEffect(bool bIsHit, FVector EndPoint, EPhysicalSurface HitSurFaceType);

	virtual void Fire() override;
};
