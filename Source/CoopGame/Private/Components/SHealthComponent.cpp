// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "SGameMode.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	// PrimaryComponentTick.bCanEverTick = true;

	bIsDead = false;

	DefaultHealth = 100.0f;

	TeamNum = 255;

	SetIsReplicated(true);
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// 由服务器计算血量变化
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();

		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::OnTakeAnyDamage);
		}
	}

	Health = DefaultHealth;
}


void USHealthComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	if (DamagedActor != DamageCauser && IsFriendly(DamagedActor, DamageCauser))
	{
		return;
	}

	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	if (Health <= 0.0f)
	{
        bIsDead = true;
	}

	if (bIsDead)
	{
		ASGameMode* GS = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
		
		if (GS)
		{
			GS->OnActorKilled.Broadcast(GetOwner(),DamagedActor, InstigatedBy);
		}
	}

	// 现在的写法需要放在上面的消息之后，因为在计分时会判断是否为playercontrolled，而在这个广播中会unpossess
	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
	// UE_LOG(LogTemp, Log, TEXT("Now Health: %f"), Health);
}

// Called every frame
//void USHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//    // ...
//}

void USHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = OldHealth - Health;
	
	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

void USHealthComponent::OnHeal(AActor* HealActor, float DeltaHealth)
{
	if (DeltaHealth < 0.0f)
	{
		return;
	}

	Health = FMath::Clamp(Health + DeltaHealth, 0.0f, DefaultHealth);

	OnHealthChanged.Broadcast(this, Health, -DeltaHealth, nullptr, nullptr, nullptr);
}

float USHealthComponent::GetHealth() const
{
	return Health;
}

bool USHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if (!ActorA || !ActorB)
	{
		return false;
	}

    USHealthComponent* HealthCompA = Cast<USHealthComponent>(ActorA->GetComponentByClass(USHealthComponent::StaticClass()));
    USHealthComponent* HealthCompB = Cast<USHealthComponent>(ActorB->GetComponentByClass(USHealthComponent::StaticClass()));
	

	if (!HealthCompA || !HealthCompB)
	{
		return false;
	}

	return HealthCompA->TeamNum == HealthCompB->TeamNum;
}

void USHealthComponent::GetLifetimeReplicatedProps( TArray< class FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(USHealthComponent, Health);
}

