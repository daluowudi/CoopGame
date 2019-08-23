// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

class UStaticMeshComponent;
class USHealthComponent;
class UMaterialInstanceDynamic;
class UParticleSystem;
class USphereComponent;
class USoundCue;

UCLASS()
class COOPGAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()
public:
	// Sets default values for this pawn's properties
	ASTrackerBot();
	// visibility
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* OverlapComp;

	// 用于判断附近有己方单位则升级伤害的组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USphereComponent* LevelupOverlapComp;

	UPROPERTY(EditDefaultsOnly, Category = "Explode")
	UParticleSystem* ExplodeEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Explode")
	int MaxLevel;

	int OverlapNum;

	UPROPERTY(EditDefaultsOnly, Category = "Explode")
	float ExplodeDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Explode")
	float ExplodeRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Explode")
	float SelfKillDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Explode")
	float ExtraDamageRatio;

	UPROPERTY(EditDefaultsOnly, Category = "Explode")
	float SelfKillInterval;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* ExplodeSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* WarningSound;

	UMaterialInstanceDynamic* MaterialInst;

	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category="Move")
	float ForceRate;

	UPROPERTY(EditDefaultsOnly, Category="Move")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category="Move")
	float TriggerDistance;

	bool bExplode;
	bool bKillingSelf;

	FTimerHandle TimerHandler;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category="Navigation")
	FVector GetNextPathPoint();

	UFUNCTION()
	void OnTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
	void SelfDestruction();

	void ApplySelfDamage();

	// virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	UFUNCTION()
	void OnSelfDestructionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnJudgeLevelupBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnJudgeLevelupEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category="Explode")
	int GetCurLevel();

	void CheckMaterialInst();
	void OnCurLevelChanged();
};
