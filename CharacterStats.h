// Richard Hill

#pragma once
#include "SecondaryStats.h"

#include "CharacterStats.generated.h"

class APlayerCharacter;

enum class ECharacterStatType : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUp, int, StatPointGain);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGainExp, int, ExperienceGained);
USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY();
	
	// Base stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseStats)
	double Strength = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseStats)
	double Agility = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseStats)
	double Intelligence = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseStats)
	double Dexterity = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseStats)
	double Vitality = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseStats)
	double Luck = 1;

	UPROPERTY(BlueprintReadWrite)
	int PointsInStr = 0;
	UPROPERTY(BlueprintReadWrite)
	int PointsInAgi = 0;
	UPROPERTY(BlueprintReadWrite)
	int PointsInInt = 0;
	UPROPERTY(BlueprintReadWrite)
	int PointsInDex = 0;
	UPROPERTY(BlueprintReadWrite)
	int PointsInVit = 0;
	UPROPERTY(BlueprintReadWrite)
	int PointsInLuk = 0;
	
	int UpgradedMeleeAtk = 0;
	int UpgradedRangedAtk = 0;
	int UpgradedMagicAtk = 0;
	int UpgradedHit = 0;
	int UpgradedArmor = 0;
	int UpgradedCrit = 0;
	int UpgradedAtkSpeed = 0;

	// Default stats
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DefaultStats)
	double DefaultMeleeAttack = 10;
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DefaultStats)
	double DefaultRangedAttack = 10;
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DefaultStats)
	double DefaultMagicAttack = 10;
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DefaultStats)
	double DefaultHit = 10;
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DefaultStats)
	double DefaultArmor = 10;
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DefaultStats)
	double DefaultCritical = 10;
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DefaultStats)
	double DefaultAtkSpeed = 1;

	// Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	double MeleeAttack = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	double RangedAttack = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	double MagicAttack = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	double Hit = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	double Armor = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	double Critical = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	double AtkSpeed = 100;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
	int AvailableStatPoints = 4;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int StatPointGainPerLevel = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Experience)
	int CurrentLevel = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Experience)
	float CurrentExp = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Experience)
	float ExpToLevel = 100;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Experience)
	float ExpReqGrowth = 10;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Experience)
	float ExpReqGrowthExponential = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Experience)
	float ExpWorth = 1;

	void InitializeStats(APlayerCharacter* Player);
	void ResetStats();
	void UpgradeStat(APlayerCharacter* Player, int UpgradeAmount, ECharacterStatType StatType, bool FromEquipment);
	void UpgradeSecondaryStat(APlayerCharacter* Player, int UpgradeAmount, ESecondaryStats SecondaryStat);
	FOnLevelUp OnLevelUp;
	FOnGainExp OnGainExp;
};
