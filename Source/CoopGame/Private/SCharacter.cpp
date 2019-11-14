// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SWeaponBase.h"
#include "CoopGame.h"
#include "SGameMode.h"
#include "SHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"

// #include "GameFramework/NavMovementComponent.h"
// #include "AI/Navigation/NavigationTypes.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("ArmComp"));
	ArmComp->SetupAttachment(RootComponent);
	ArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(ArmComp);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	//这个如果不说是很难发现的
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

  	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WEAPON, ECR_Ignore);

	ZoomedFOV = 65;
	ZoomSpeed = 20;

	IsDied = false;
    bReloading = false;

	WeaponSocketName = "WeaponSocket";

	// SetReplicates(true);
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (HealthComp)
	{
		// HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
		HealthComp->FOnCharacterDied.AddDynamic(this, &ASCharacter::OnCharacterDied);
	}

	DefaultsFOV = CameraComp->FieldOfView;

	if (HasAuthority())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CurrentWeapon = GetWorld()->SpawnActor<ASWeaponBase>(StaterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		if (CurrentWeapon)
		{
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);

            OnWeaponChanged.Broadcast(this, nullptr, CurrentWeapon);
		}
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	float TargetFOV = bIsZoomed ? ZoomedFOV : DefaultsFOV;

	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomSpeed);
	CameraComp->SetFieldOfView(NewFOV);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);	
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);	

	PlayerInputComponent->BindAxis("Lookup", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("UnCrouch", IE_Released, this, &ASCharacter::EndCrouch);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::DoZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::UnZoom);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASCharacter::DoReload);
	// PlayerInputComponent->BindAction("Reload", IE_Released, this, &ASCharacter::UnZoom);
}

void ASCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();	
	}
}

void ASCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void ASCharacter::ServerReload_Implementation()
{
	DoReload();
}

bool ASCharacter::ServerReload_Validate()
{
	return true;
}

void ASCharacter::DoReload()
{
	// if (bReloading) return;

	if (!HasAuthority())
	{
		ServerReload();
	}

	bReloading = true;

	float duration = PlayAnimMontage(ReloadAnim);

	if (HasAuthority())
	{
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ASCharacter::FinishReload, duration, false);
	}
}

void ASCharacter::FinishReload()
{
	// if (!bReloading) return;

	StopAnimMontage(ReloadAnim);

	bReloading = false;
}

void ASCharacter::OnRep_bReloading(bool OldReloading)
{
	if (bReloading)
	{
		DoReload();
	}else
	{
		FinishReload();
	}
}

void ASCharacter::OnRep_CurrentWeapon(ASWeaponBase* OldWeapon)
{
	OnWeaponChanged.Broadcast(this, OldWeapon, CurrentWeapon);
}

void ASCharacter::DoZoom()
{
	bIsZoomed = true;
}

void ASCharacter::UnZoom()
{
	bIsZoomed = false;
}

void ASCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

void ASCharacter::BeginCrouch()
{
	Crouch();
}

void ASCharacter::EndCrouch()
{
	UnCrouch();
}

FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

void ASCharacter::OnCharacterDied(class AController* InstigatedBy, AActor* DamageCauser)
{
	if (IsDied) return;
	
	IsDied = true;

	// 计分
	ASGameMode* GS = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
	if (GS)
		GS->OnActorKilled.Broadcast(this, this, InstigatedBy);

	// 需要停火
	StopFire();

	GetMovementComponent()->StopMovementImmediately();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/*
		延迟处理controlelr unprocess原因是
		如果直接在这里调用 controller会直接在server unprocess 
		虽然OnHealthChanged.Broadcast 发生在 FOnCharacterDied.Broadcast
		但是服务端DetachFromControllerPendingDestroy的执行依然早于 blueprint收到OnHealthChanged（blueprint会直接收不到最后一次血量变化）
		假设每次减少10点血 最终 HUD端的显示会剩下10的血 但是人物已经挂了
		尝试了几种调整都没有显著作用，加个注释在这里存疑吧
	*/ 
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASCharacter::RealDied, 0.1, false);

	UE_LOG(LogTemp, Log, TEXT("Now CharacterDied"));

	// UE_LOG(LogTemp, Log, TEXT("Now Health: %f"), Health);
}

void ASCharacter::RealDied()
{
	DetachFromControllerPendingDestroy();
	SetLifeSpan(1);
}

void ASCharacter::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	// if (Health <= 0.0f)
 //        FOnCharacterDied.Broadcast(InstigatedBy, DamageCauser);
}

float ASCharacter::PlayAnimMontage(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	USkeletalMeshComponent* UseMesh = GetMesh();
	if (AnimMontage && UseMesh && UseMesh->AnimScriptInstance)
	{
		return UseMesh->AnimScriptInstance->Montage_Play(AnimMontage, InPlayRate);
	}

	return 0.0f;
}

void ASCharacter::StopAnimMontage(UAnimMontage* AnimMontage)
{
	USkeletalMeshComponent* UseMesh = GetMesh();
	if (AnimMontage && UseMesh && UseMesh->AnimScriptInstance
		&& UseMesh->AnimScriptInstance->Montage_IsPlaying(AnimMontage))
	{
		UseMesh->AnimScriptInstance->Montage_Stop(AnimMontage->BlendOut.GetBlendTime(), AnimMontage);
	}
}

void ASCharacter::GetLifetimeReplicatedProps( TArray< class FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(ASCharacter, CurrentWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ASCharacter, IsDied);
	DOREPLIFETIME_CONDITION(ASCharacter, bReloading, COND_SkipOwner);
}
