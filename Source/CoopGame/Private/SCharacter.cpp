// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SWeapon.h"
#include "CoopGame.h"
#include "SHealthComponent.h"
#include "Components/CapsuleComponent.h"

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

	bDied = false;

	WeaponSocketName = "WeaponSocket";
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (HealthComp)
	{
		HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
	}

	DefaultsFOV = CameraComp->FieldOfView;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(StaterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (CurrentWeapon)
	{
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
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

void ASCharacter::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bDied)
	{
		bDied = true;

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();

		SetLifeSpan(5);
	}
	UE_LOG(LogTemp, Log, TEXT("Now Health: %f"), Health);
}