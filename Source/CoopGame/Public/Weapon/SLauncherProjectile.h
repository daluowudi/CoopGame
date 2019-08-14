// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLauncherProjectile.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;
class UParticleSystem;
class USoundCue;

UCLASS()
class COOPGAME_API ASLauncherProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASLauncherProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	void OnProjectileExplode();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerExplode();
protected:
	UProjectileMovementComponent* MovementComp;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, Category="Effect")
	UParticleSystem* ExplodeEffect;

	UPROPERTY(EditDefaultsOnly, Category="Sound")
	USoundCue* ExplodeSound;

	UPROPERTY(EditDefaultsOnly, Category="Projectile", meta=(ClampMin="0", ClampMax="1"))
	float GravityScale;

	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	float BoomDelay;

	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	float ExplodeRadius;

	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	float ExplodeDamage;

	AActor* DamageCauser;
public:	
	// Called every frame
	// virtual void Tick(float DeltaTime) override;

	// 发射!
	void Launch(FVector Velocity, AActor* Causer);

	// 爆炸倒计时
	FTimerHandle BoomTimerHandler;
};
