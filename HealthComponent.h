// Richard Hill

#pragma once

#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class UHealthWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnTakeDamageDelegate, float, Damage, bool, bIsCrit, bool, bIsBackStab, AActor*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UCLASS(meta=(BlueprintSpawnableComponent))
class UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Health")
	float CurrentHealth = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	float BaseMaxHealth = 100;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Health")
	float MaxHealth = 100;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Health")
	float ImmunityTime = 1;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category="Health")
	float HealthGainPerSec = 1;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category="Health")
	float VitalityHealthMultiplier = 10;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Health")
	bool bIsPlayer = false;
	
	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnTakeDamageDelegate OnHit;
	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnDeath OnDeath;
	UFUNCTION(BlueprintCallable)
	void TakeDamage(float IncomingHealth, bool IsCritHit, AActor* Instigator);
	UFUNCTION(BlueprintCallable)
	void GainHealth(int IncomingHealth);
	UFUNCTION(Server, Reliable)
	void Server_GainHealth(int IncomingHealth);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_GainHealth(int NewCurrentHealth);
	UFUNCTION()
	bool GetIsImmune() const;
	UFUNCTION(BlueprintCallable)
	void Death();
	UFUNCTION()
	void DashImmunity(bool EnableImmunity);
	UFUNCTION()
	void IncreaseVitality(float Vitality, int PlayerLevel);
	UFUNCTION(Server, Reliable)
	void Server_IncreaseVitality(float Vitality, int PlayerLevel);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_IncreaseVitality(float Vitality, int PlayerLevel);
	UFUNCTION()
	bool GetIsDead();
	UFUNCTION(BlueprintCallable)
	void SetIsDead(bool InBool);
	

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void GainHealthPerSec(float DeltaTime);

	UPROPERTY()
	AActor* Owner;
	UPROPERTY()
	float ImmunityCountDown;
	UPROPERTY(BlueprintReadOnly)
	bool bIsImmune;
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsDead;

	UFUNCTION()
	void HandleImmunityTimer(float DeltaTime);

};
