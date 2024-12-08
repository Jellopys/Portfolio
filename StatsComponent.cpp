// Richard Hill


#include "StatsComponent.h"

#include "SecondaryStats.h"
#include "SchoolTesting/Player/PlayerCharacter.h"
#include "SchoolTesting/Player/CustomComponents/HealthComponent.h"
#include "SchoolTesting/Player/CustomComponents/Magic/ManaComponent.h"
#include "Widgets/Text/ISlateEditableTextWidget.h"


UStatsComponent::UStatsComponent()
{

}


void UStatsComponent::BeginPlay()
{
	Super::BeginPlay();
	// TODO: Load Stats from save

	
	Player = Cast<APlayerCharacter>(GetOwner());

	if (Player)
	{
		bIsPlayer = true;
		
		FTimerHandle TimerHandle; 
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UStatsComponent::InitializeStats, 0.1f, false);
	}
}

void UStatsComponent::InitializeStats()
{
	CharacterStats.InitializeStats(Player);
	Player->ManaComponent->IncreaseIntelligence(CharacterStats.Intelligence, CharacterStats.CurrentLevel);
	Player->ManaComponent->CurrentMana = Player->ManaComponent->MaxMana;
	Player->HealthComponent->IncreaseVitality(CharacterStats.Vitality, CharacterStats.CurrentLevel);
	Player->HealthComponent->CurrentHealth = Player->HealthComponent->MaxHealth;
}

void UStatsComponent::OnLevelUpCallback(int AvailablePoints)
{
	OnLevelUp_BP.Broadcast(AvailablePoints);
	Player->HealthComponent->IncreaseVitality(CharacterStats.Vitality, CharacterStats.CurrentLevel);
	Player->ManaComponent->IncreaseIntelligence(CharacterStats.Intelligence, CharacterStats.CurrentLevel);
}

void UStatsComponent::OnGainExpCallback(int Experience)
{
	OnGainExp_BP.Broadcast(Experience);
}

void UStatsComponent::InitStats()
{
	
}

void UStatsComponent::GainStats()
{
	
}

void UStatsComponent::ResetStats()
{
	CharacterStats.ResetStats();
	
	Player->ManaComponent->IncreaseIntelligence(CharacterStats.Intelligence, CharacterStats.CurrentLevel);
	Player->HealthComponent->IncreaseVitality(CharacterStats.Vitality, CharacterStats.CurrentLevel);
	Player->HealthComponent->CurrentHealth = Player->HealthComponent->MaxHealth;
	Player->ManaComponent->CurrentMana = Player->ManaComponent->MaxMana;
}

bool UStatsComponent::GetIsCrit()
{
	float const LocalCrit = FMath::Clamp(CharacterStats.Luck, 0, 100);

	float const RandomRoll = FMath::RandRange(0, 100);

	if (RandomRoll <= LocalCrit)
	{
		return true;
	}
	return false;
}

void UStatsComponent::UpgradeStat(int UpgradeAmount, ECharacterStatType StatType, bool bRemoveStat)
{
	CharacterStats.UpgradeStat(Player, UpgradeAmount, StatType, bRemoveStat);
}

void UStatsComponent::UpgradeSecondaryStat(int UpgradeAmount, ESecondaryStats SecondaryStat)
{
	CharacterStats.UpgradeSecondaryStat(Player, UpgradeAmount, SecondaryStat);
}

void UStatsComponent::GainExp(int ExpAmount)
{
	CharacterStats.CurrentExp += ExpAmount;
	OnGainExpCallback(ExpAmount);
	if (CharacterStats.CurrentExp >= CharacterStats.ExpToLevel)
	{
		float const RestExperience = CharacterStats.CurrentExp - CharacterStats.ExpToLevel;
		LevelUp(RestExperience);
	}
}

void UStatsComponent::LevelUp(int RestAmount)
{	
	CharacterStats.CurrentLevel += 1;
    CharacterStats.CurrentExp = 0;
    CharacterStats.ExpToLevel = FMath::Pow(CharacterStats.CurrentLevel * CharacterStats.ExpReqGrowth, CharacterStats.ExpReqGrowthExponential);
    CharacterStats.AvailableStatPoints += CharacterStats.StatPointGainPerLevel;
	
	OnLevelUpCallback(CharacterStats.AvailableStatPoints);
    GainExp(RestAmount);
}





