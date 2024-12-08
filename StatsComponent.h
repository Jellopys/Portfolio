// Richard Hill

#pragma once

#include "SchoolTesting/Player/CustomComponents/Stats/CharacterStats.h"
#include "Components/ActorComponent.h"
#include "StatsComponent.generated.h"

// struct FCharacterStats;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUp_BP, int, AvailablePoints);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGainExp_BP, int, Experience);
UCLASS(meta=(BlueprintSpawnableComponent))
class SCHOOLTESTING_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatsComponent();
	virtual void BeginPlay() override;
	void InitializeStats();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseStats)
	FCharacterStats CharacterStats;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class APlayerCharacter* Player;

	UFUNCTION(BlueprintCallable)
	int GetCurrentLevel() { return CharacterStats.CurrentLevel; }
	UFUNCTION(BlueprintCallable)
	void InitStats();
	UFUNCTION(BlueprintCallable)
	void GainStats();
	UFUNCTION(BlueprintCallable)
	void ResetStats();
	UFUNCTION()
	bool GetIsCrit();
	UFUNCTION(BlueprintCallable)
	void UpgradeStat(int UpgradeAmount, ECharacterStatType StatType, bool bRemoveStat);
	UFUNCTION(BlueprintCallable)
	void UpgradeSecondaryStat(int UpgradeAmount, ESecondaryStats SecondaryStat);
	void GainExp(int ExpAmount);
	void LevelUp(int RestAmount);
	UPROPERTY(BlueprintAssignable)
	FOnLevelUp_BP OnLevelUp_BP;
	UPROPERTY(BlueprintAssignable)
	FOnGainExp_BP OnGainExp_BP;

private:
	UFUNCTION()
	void OnLevelUpCallback(int AvailablePoints);
	UFUNCTION()
	void OnGainExpCallback(int Experience);

protected:
	bool bIsPlayer = false;
};
