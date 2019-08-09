// Fill out your copyright notice in the Description page of Project Settings.

#include "STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "SHealthComponent.h"
#include "SCharacter.h"
#include "Sound/SoundCue.h"


// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);

	OverlapComp = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapComp"));
	OverlapComp->SetupAttachment(RootComponent);
	OverlapComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	ForceRate = 1000.0f;
	bUseVelocityChange = false;

	bExplode = false;
	ExplodeDamage = 50.0f;
	ExplodeRadius = 50.0f;
	SelfKillDamage = 20.0f;
	SelfKillInterval = 0.5f;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComp)
	{
		HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::OnTakeDamage);
	}
	
	if (HasAuthority())
	{
		NextPathPoint = GetNextPathPoint();
	}
}

FVector ASTrackerBot::GetNextPathPoint()
{
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	APawn* TargetPawn = nullptr;
	float NearstDistance = 0.0f;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* ItPawn = It->Get();
		if (IsValid(ItPawn) && !USHealthComponent::IsFriendly(ItPawn, this))
		{
			USHealthComponent* HealthComponent = Cast<USHealthComponent>(ItPawn->GetComponentByClass(USHealthComponent::StaticClass()));

			if (HealthComponent && HealthComponent->GetHealth() > 0.0f)
			{
				float DistanceToTarget = (ItPawn->GetActorLocation() - this->GetActorLocation()).Size();
				if (!TargetPawn || DistanceToTarget < NearstDistance)
				{
					NearstDistance = DistanceToTarget;
					TargetPawn = ItPawn;
				}
			}
		}
	}

	if (TargetPawn)
	{
		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), TargetPawn);

		if (NavPath && NavPath->PathPoints.Num() > 1)
		{
			return NavPath->PathPoints[1];
		}
	}
	
	return GetActorLocation();
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && !bExplode)
	{
		float Distance = (NextPathPoint - GetActorLocation()).Size();

		if (Distance <= TriggerDistance)
		{
			NextPathPoint = GetNextPathPoint();
		}else
		{
			FVector ForceDirection = NextPathPoint - GetActorLocation();

			ForceDirection *= ForceRate;

			MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
		}	
	}
}

void ASTrackerBot::OnTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (!MaterialInst)
	{
		MaterialInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	
	if (MaterialInst)
	{
		MaterialInst->SetScalarParameterValue("LastHittedTime", GetWorld()->GetTimeSeconds());
	}	
	UE_LOG(LogTemp, Log, TEXT("Now TrackerBot Health: %f"), Health);	

	if (Health <= 0.0)
	{
		SelfDestruction();
	}
}

void ASTrackerBot::SelfDestruction()
{
	if (bExplode)
	{
		return;
	}

	bExplode = true;
	// effect
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplodeEffect, GetActorLocation());

	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation(), FRotator::ZeroRotator);

	MeshComp->SetVisibility(false, false);
	// MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (HasAuthority())
	{
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(this);

		// damage
		UGameplayStatics::ApplyRadialDamage(this, ExplodeDamage, GetActorLocation(), ExplodeRadius, nullptr, IgnoreActors, this);
		// debug sphere
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), GetActorLocation(), ExplodeRadius, 8, FColor::Red, 5.0f);


		SetLifeSpan(2.0f);
	}	
}

void  ASTrackerBot::ApplySelfDamage()
{
	if (!bExplode)
	{
		UGameplayStatics::ApplyDamage(this, SelfKillDamage, GetController(), this, nullptr);
	}
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);
	// 不是玩家则返回
	if (!PlayerPawn)
	{
		return;
	}

	if (USHealthComponent::IsFriendly(this, OtherActor))
	{
		return;
	}

	if (bKillingSelf)
	{
		return;
	}
	bKillingSelf = true;

	if (HasAuthority() && !bExplode)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandler, this, &ASTrackerBot::ApplySelfDamage, SelfKillInterval, true);
	}

	UGameplayStatics::SpawnSoundAttached(WarningSound, RootComponent);
}