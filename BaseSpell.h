// Richard Hill

#pragma once

#include "GameFramework/Actor.h"
#include "BaseSpell.generated.h"

enum class EDamageStatType : uint8;
enum class EWeaponType : uint8;
class APlayerCharacter;
class UManaComponent;

UCLASS()
class ABaseSpell : public AActor
{
	GENERATED_BODY()

public:
	ABaseSpell();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Activate();
	UFUNCTION(Server, Reliable)
	virtual void Server_Activate();
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multi_Activate();
	virtual void Release();
	UFUNCTION(Server, Reliable)
	virtual void Server_Release();
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multi_Release();
	virtual void SetOnCooldown(bool InBool);
	UFUNCTION(Server, Reliable)
	virtual void SetSpellOwner(AActor* InSpellOwner);
	UFUNCTION(BlueprintCallable)
	virtual void CancelAttack();
	UFUNCTION(BlueprintImplementableEvent)
	void OnCancel();
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multi_Cancel();
	UFUNCTION(BlueprintCallable)
	virtual float GetDamage(bool& IsCrit);
	virtual bool GetIsCrit();
	bool GetIsOnCooldown();
	void UseMana();
	UFUNCTION()
	void OnHit();

	UPROPERTY(EditAnywhere)
	float BaseDamage = 10;
	UPROPERTY(EditAnywhere)
	float Cooldown = 0.5f;
	float CooldownTimer = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWeaponType RequiredWeaponType;

	// UPROPERTY(Replicated, BlueprintReadWrite)
	// APlayerCharacter* PlayerCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EDamageStatType, float> DamageStatMap;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString SpellName = "Spell Name";
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ManaCost = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture* SpellIcon;

protected:
	UPROPERTY(Replicated, BlueprintReadOnly)
	AActor* SpellOwner;
	UPROPERTY()
	UManaComponent* ManaComponent;
};
