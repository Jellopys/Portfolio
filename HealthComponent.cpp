// Richard Hill


#include "HealthComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "SchoolTesting/Enemy/BaseEnemy.h"
#include "SchoolTesting/Helpers/ContextHelper.h"
#include "SchoolTesting/Player/PlayerCharacter.h"
#include "SchoolTesting/Widgets/EnemyHealthBarWidget.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	SetIsReplicatedByDefault(true);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();
	ImmunityCountDown = ImmunityTime;
	
	CurrentHealth = MaxHealth;
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	HandleImmunityTimer(DeltaTime);
	GainHealthPerSec(DeltaTime);
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, CurrentHealth);
	DOREPLIFETIME(UHealthComponent, MaxHealth);
	DOREPLIFETIME(UHealthComponent, bIsDead);
	DOREPLIFETIME(UHealthComponent, HealthGainPerSec);
	DOREPLIFETIME(UHealthComponent, VitalityHealthMultiplier);
}

void UHealthComponent::HandleImmunityTimer(float DeltaTime)
{
	if (!bIsImmune) return;
	if (bIsDead) return;
	
	ImmunityCountDown -= DeltaTime;
	if (ImmunityCountDown <= 0)
	{
		bIsImmune = false;
		ImmunityCountDown = ImmunityTime; 
	}
}

void UHealthComponent::GainHealthPerSec(float DeltaTime)
{
	if (!IsValid(Owner) || !Owner->HasAuthority()) return;
	
	if (CurrentHealth >= MaxHealth)
	{
		return;
	}
	if (bIsDead) return;

	CurrentHealth += HealthGainPerSec * DeltaTime;
}

void UHealthComponent::TakeDamage(float IncomingHealth, bool IsCritHit = false, AActor* Instigator = nullptr)
{
	if (bIsImmune) return;

	bool bIsBackStab = false;

	if (Instigator) // Checks backstab
	{
		uint8 const RelativeContext = UContextHelper::GetRelativeContext(Owner, Instigator);
		
		if(UContextHelper::CheckContext(RelativeContext, static_cast<int8>(ERelativeContext::FacingSame)))
		{
			IncomingHealth *= CONST_BACKSTAB_MULTIPLIER;
			bIsBackStab = true;
		}
	}

	// Count armor here
	if (GetOwner()->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
	{
		const UStatsComponent* Stats = ICharacterInterface::Execute_GetStatsComponent(GetOwner());

		if (Instigator && Instigator->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
		{
			const UStatsComponent* InstigatorStats = ICharacterInterface::Execute_GetStatsComponent(Instigator);
			float const ArmorPenetration = Stats->CharacterStats.Armor - InstigatorStats->CharacterStats.Hit;
			IncomingHealth = FMath::Clamp(IncomingHealth - ArmorPenetration, 0, 9999);
		}
		
	}
	
	float const FinalDamage = IncomingHealth;

	if (bIsPlayer)
	{
		CurrentHealth = FMath::Clamp(CurrentHealth - FinalDamage, 0, MaxHealth);
		// bIsImmune = true;
	}
	else
	{
		CurrentHealth = FMath::Clamp(CurrentHealth - FinalDamage, 0, MaxHealth);
	}
	
	OnHit.Broadcast(FinalDamage, IsCritHit, bIsBackStab, Instigator);
	
	if (CurrentHealth <= 0)
	{
		Death();
	}
}

void UHealthComponent::GainHealth(int IncomingHealth)
{
	Server_GainHealth(IncomingHealth);
}

void UHealthComponent::Server_GainHealth_Implementation(int IncomingHealth)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + IncomingHealth, 0, MaxHealth);
	Multi_GainHealth(CurrentHealth);
}

void UHealthComponent::Multi_GainHealth_Implementation(int NewCurrentHealth)
{
	CurrentHealth = NewCurrentHealth;
}

bool UHealthComponent::GetIsImmune() const
{
	return bIsImmune;
}

void UHealthComponent::Death()
{
	if (bIsDead) return;
	bIsDead = true;
	
	if (!bIsPlayer) // If enemy dies
	{
		OnDeath.Broadcast();
		// Bad implementation, Refactor this to interface message call instead
		ABaseEnemy* EnemyRef = Cast<ABaseEnemy>(Owner);
		if (!EnemyRef) return;
		EnemyRef->TriggerDeath();
		float const ExpToGive = EnemyRef->StatsComponent->CharacterStats.ExpWorth;

		// Bad implementation, Refactor this to interface message call instead
		APlayerCharacter* const PlayerRef = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (!PlayerRef) return;
		PlayerRef->StatsComponent->GainExp(ExpToGive);
		PlayerRef->TryUnTargetDyingEnemy(EnemyRef);
	}
	else // if Player dies
	{
		OnDeath.Broadcast();
	}
}

void UHealthComponent::DashImmunity(bool EnableImmunity)
{
	if (EnableImmunity)
	{
		ImmunityCountDown = 9999;
	}
	else
	{
		ImmunityCountDown = 0;
	}
	bIsImmune = EnableImmunity;
}


void UHealthComponent::IncreaseVitality(float Vitality, int PlayerLevel)
{
	Server_IncreaseVitality(Vitality, PlayerLevel);
}

void UHealthComponent::Server_IncreaseVitality_Implementation(float Vitality, int PlayerLevel)
{
	float const HPFromVitality = Vitality * VitalityHealthMultiplier;
	MaxHealth = BaseMaxHealth + HPFromVitality + (PlayerLevel * 10) + PlayerLevel;
	HealthGainPerSec = 1 + (Vitality * 0.5f);
}

void UHealthComponent::Multi_IncreaseVitality_Implementation(float Vitality, int PlayerLevel)
{
	
}

bool UHealthComponent::GetIsDead()
{
	return bIsDead;
}

void UHealthComponent::SetIsDead(bool InBool)
{
	bIsDead = InBool;
}
