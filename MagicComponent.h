// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseSpell.h"
#include "Components/ActorComponent.h"
#include "MagicComponent.generated.h"

class ABaseSpell;
class UMaterialInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeSpell, int, SpellBookIndex);
UCLASS(meta=(BlueprintSpawnableComponent))
class UMagicComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMagicComponent();
	UFUNCTION(BlueprintCallable)
	void PrimaryFire(EWeaponType WeaponType);
	UFUNCTION(BlueprintCallable)
	void PrimaryRelease();
	UFUNCTION(BlueprintCallable)
	void SecondaryFire();
	UFUNCTION(BlueprintCallable)
	void ActivateMovementSpell();
	UFUNCTION(BlueprintCallable)
	void CancelAttacks();
	
	UFUNCTION()
	void ChangeSpell(int SpellBookIndex);

	UFUNCTION()
	void SetCastbarPercentage(float Percentage);

	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnChangeSpell OnChangeSpell;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TArray<TSubclassOf<ABaseSpell>> AvailableSpells;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TArray<ABaseSpell*> SpawnedSpells;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TMap<EWeaponType, TSubclassOf<ABaseSpell>> BasicSpells;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TMap<EWeaponType, ABaseSpell*> SpawnedBasicSpells;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	ABaseSpell* CurrentSpell;
	
	UPROPERTY()
	ABaseSpell* PrimarySpell;
	UPROPERTY()
	ABaseSpell* SecondarySpell;
	UPROPERTY()
	ABaseSpell* MovementSpell;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mana)
	float CurrentMana = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mana)
	float MaxMana = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mana)
	float ManaGainPerSecond = 1;

	UPROPERTY(BlueprintReadOnly)
	float CastbarPercentage = 0;

	UFUNCTION(BlueprintCallable)
	void GainMana(int IncomingMana);
	UFUNCTION(BlueprintCallable)
	bool UseMana(int ManaCost);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void TickManaPerSec(float DeltaTime);
private:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseSpell> PrimarySpellClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseSpell> SecondarySpellClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseSpell> MovementSpellClass;

	bool bCanChangeSpell = false;
};
