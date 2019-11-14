// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWeaponChangedSignature, class AActor*, Owner, class ASWeaponBase*, OldWeapon, class ASWeaponBase*, NewWeapon);

class UCameraComponent;
class USpringArmComponent;
class ASWeaponBase;
class USHealthComponent;
class UAnimMontage;

UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void BeginCrouch();

	void EndCrouch();

	void DoZoom();

	void UnZoom();

	void DoReload();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReload();

	void FinishReload();

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnCharacterDied(class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* ArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	float ZoomedFOV;
	
	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomSpeed;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentWeapon, VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	ASWeaponBase* CurrentWeapon;

	UFUNCTION()
	void OnRep_CurrentWeapon(ASWeaponBase* OldWeapon);

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ASWeaponBase> StaterWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	UAnimMontage* ReloadAnim;

	bool bIsZoomed;

	float DefaultsFOV;

	FName WeaponSocketName;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWeaponChangedSignature OnWeaponChanged;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool IsDied;

	UPROPERTY(ReplicatedUsing=OnRep_bReloading, BlueprintReadOnly, Category = "Player")
	bool bReloading;

	UFUNCTION()
	void OnRep_bReloading(bool OldReloading);

	UFUNCTION(BlueprintCallable, Category="Weapon")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category="Weapon")
	void StopFire();

	void RealDied();

	virtual float PlayAnimMontage(UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;

	virtual void StopAnimMontage(UAnimMontage* AnimMontage) override;
};
