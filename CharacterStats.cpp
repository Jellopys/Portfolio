#include "CharacterStats.h"

#include "CharacterStatType.h"
#include "SecondaryStats.h"
#include "SchoolTesting/Player/PlayerCharacter.h"
#include "SchoolTesting/Player/CustomComponents/HealthComponent.h"
#include "SchoolTesting/Player/CustomComponents/Magic/ManaComponent.h"

void FCharacterStats::InitializeStats(APlayerCharacter* Player)
{
	MeleeAttack = DefaultMeleeAttack + (Strength) + (Luck * 0.5f) + UpgradedMeleeAtk;
	MagicAttack = DefaultMagicAttack + (Intelligence) + (Luck * 0.5f) + UpgradedMagicAtk;
	RangedAttack = DefaultRangedAttack + (Dexterity) + (Luck * 0.5f) + UpgradedRangedAtk;
	AtkSpeed = DefaultAtkSpeed + (Agility * 0.005f) + (UpgradedAtkSpeed * 0.005f);
	Hit = DefaultHit + (Dexterity * 0.5f) + (Luck * 0.5f) + UpgradedHit;
	Armor = DefaultArmor + (Vitality * 0.5f) + (Agility * 0.5f) + UpgradedArmor;
	Critical = DefaultCritical + (Luck * 0.5f) + UpgradedCrit;
	
	ExpToLevel = FMath::Pow(CurrentLevel * ExpReqGrowth, ExpReqGrowthExponential);

	if (Player)
	{
		Player->ManaComponent->IncreaseIntelligence(Intelligence, CurrentLevel);
		Player->HealthComponent->IncreaseVitality(Vitality, CurrentLevel);
		Player->HealthComponent->CurrentHealth = Player->HealthComponent->MaxHealth;
		Player->ManaComponent->CurrentMana = Player->ManaComponent->MaxMana;
	}
}

void FCharacterStats::ResetStats()
{
	Strength -= PointsInStr;
	Agility -= PointsInAgi; 
	Intelligence -= PointsInInt;
	Dexterity -= PointsInDex;
	Vitality -= PointsInVit;
	Luck -= PointsInLuk;

	PointsInStr = 0;
	PointsInAgi = 0;
	PointsInInt = 0;
	PointsInDex = 0;
	PointsInVit = 0;
	PointsInLuk = 0;
	AvailableStatPoints = StatPointGainPerLevel * CurrentLevel;

	MeleeAttack = DefaultMeleeAttack + (Strength) + (Luck * 0.5f) + UpgradedMeleeAtk;
	MagicAttack = DefaultMagicAttack + (Intelligence) + (Luck * 0.5f) + UpgradedMagicAtk;
	RangedAttack = DefaultRangedAttack + (Dexterity) + (Luck * 0.5f) + UpgradedRangedAtk;
	AtkSpeed = DefaultAtkSpeed + (Agility * 0.005f) + (UpgradedAtkSpeed * 0.005f);
	Hit = DefaultHit + (Dexterity * 0.5f) + (Luck * 0.5f) + UpgradedHit;
	Armor = DefaultArmor + (Vitality * 0.5f) + (Agility * 0.5f) + UpgradedArmor;
	Critical = DefaultCritical + (Luck * 0.5f) + UpgradedCrit;
}

void FCharacterStats::UpgradeStat(APlayerCharacter* Player, int UpgradeAmount, ECharacterStatType StatType, bool FromEquipment)
{
	switch (StatType)
	{
	case ECharacterStatType::Strength :
		Strength += UpgradeAmount;
		MeleeAttack = DefaultMeleeAttack + (Strength) + (Luck * 0.5f) + UpgradedMeleeAtk;
		if (!FromEquipment)
			PointsInStr++;
		break;
	case ECharacterStatType::Agility :
		Agility += UpgradeAmount;
		AtkSpeed = DefaultAtkSpeed + (Agility * 0.005f) + (UpgradedAtkSpeed * 0.005f);
		Armor = DefaultArmor + (Vitality * 0.5f) + (Agility * 0.5f) + UpgradedArmor;
		if (!FromEquipment)
			PointsInAgi++;
		break;
	case ECharacterStatType::Intelligence :
		Intelligence += UpgradeAmount;
		Player->ManaComponent->IncreaseIntelligence(Intelligence, CurrentLevel);
		MagicAttack = DefaultMagicAttack + (Intelligence) + (Luck * 0.5f) + UpgradedMagicAtk;
		if (!FromEquipment)
			PointsInInt++;
		break;
	case ECharacterStatType::Dexterity :
		Dexterity += UpgradeAmount;
		Hit = DefaultHit + (Dexterity * 0.5f) + (Luck * 0.5f) + UpgradedHit;
		RangedAttack = DefaultRangedAttack + (Dexterity) + (Luck * 0.5f) + UpgradedRangedAtk;
		if (!FromEquipment)
			PointsInDex++;
		break;
	case ECharacterStatType::Vitality :
		Vitality += UpgradeAmount;
		Player->HealthComponent->IncreaseVitality(Vitality, CurrentLevel);
		Armor = DefaultArmor + (Vitality * 0.5f) + (Agility * 0.5f) + UpgradedArmor;
		if (!FromEquipment)
			PointsInVit++;
		break;
	case ECharacterStatType::Luck :
		Luck += UpgradeAmount;
		Hit = DefaultHit + (Dexterity * 0.5f) + (Luck * 0.5f) + UpgradedHit;
		MeleeAttack = DefaultMeleeAttack + (Strength) + (Luck * 0.5f) + UpgradedMeleeAtk;
		MagicAttack = DefaultMagicAttack + (Intelligence) + (Luck * 0.5f) + UpgradedMagicAtk;
		RangedAttack = DefaultRangedAttack + (Dexterity) + (Luck * 0.5f) + UpgradedRangedAtk;
		Critical = DefaultCritical + (Luck * 0.5f) + UpgradedCrit;
		if (!FromEquipment)
			PointsInLuk++;
		break;
	default:
		break;
	}

	if (!FromEquipment)
	{
		AvailableStatPoints = FMath::Clamp(AvailableStatPoints - 1, 0, 999);
	}
}


void FCharacterStats::UpgradeSecondaryStat(APlayerCharacter* Player, int UpgradeAmount, ESecondaryStats SecondaryStat)
{
	switch (SecondaryStat)
	{
	case ESecondaryStats::Armor :
		UpgradedArmor += UpgradeAmount;
		Armor = DefaultArmor + (Vitality * 0.5f) + (Agility * 0.5f) + UpgradedArmor;
		break;
	case ESecondaryStats::Critical :
		UpgradedCrit += UpgradeAmount;
		Critical = DefaultCritical + (Luck * 0.5f) + UpgradedCrit;
		break;
	case ESecondaryStats::Hit :
		UpgradedHit += UpgradeAmount;
		Hit = DefaultHit + (Dexterity * 0.5f) + (Luck * 0.5f) + UpgradedHit;
		break;
	case ESecondaryStats::AtkSpeed :
		UpgradedAtkSpeed += UpgradeAmount;
		AtkSpeed = DefaultAtkSpeed + (Agility * 0.005f) + (UpgradedAtkSpeed * 0.005f);
		break;
	case ESecondaryStats::MagicAttack :
		UpgradedMagicAtk += UpgradeAmount;
		MagicAttack = DefaultMagicAttack + (Intelligence) + (Luck * 0.5f) + UpgradedMagicAtk;
		break;
	case ESecondaryStats::MeleeAttack :
		UpgradedMeleeAtk += UpgradeAmount;
		MeleeAttack = DefaultMeleeAttack + (Strength) + (Luck * 0.5f) + UpgradedMeleeAtk;
		break;
	case ESecondaryStats::RangedAttack :
		UpgradedRangedAtk += UpgradeAmount;
		RangedAttack = DefaultRangedAttack + (Dexterity) + (Luck * 0.5f) + UpgradedRangedAtk;
		break;
		
		default:
			break;
	}
}