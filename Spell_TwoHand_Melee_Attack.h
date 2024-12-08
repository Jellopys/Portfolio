// Richard Hill

#pragma once

#include "CoreMinimal.h"
#include "SchoolTesting/Player/CustomComponents/Magic/BaseSpell.h"
#include "Spell_TwoHand_Melee_Attack.generated.h"


UCLASS()
class  ASpell_TwoHand_Melee_Attack : public ABaseSpell
{
	GENERATED_BODY()

public:
	ASpell_TwoHand_Melee_Attack();
	
	UPROPERTY(EditAnywhere)
	UAnimMontage* MeleeAttackMontage;
	UPROPERTY(EditAnywhere)
	float AttackSpeedIncrease = 0;
	UPROPERTY(EditAnywhere)
	float FullyChargedTimeBuffer = 0;
	UPROPERTY(EditAnywhere)
	float MovementSpeedDuringAttack = 200;

	UFUNCTION(BlueprintCallable)
	void UpgradeDamage(float ExtraDamage);
	UFUNCTION(BlueprintCallable)
	void UpgradeCleave(float ExtraRadius);	
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnCast();

	UFUNCTION(BlueprintImplementableEvent)
	void OnRelease();

	UFUNCTION(BlueprintCallable)
	float GetCurrentAttackTimeProgress();

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bHolding = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CleaveRadius = 0;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Server_Activate_Implementation() override;
	virtual void Server_Release_Implementation() override;
	virtual void Multi_Activate_Implementation() override;
	virtual void Multi_Release_Implementation() override;
	virtual void Multi_Cancel_Implementation() override;
	virtual void Activate() override;
	virtual void Release() override;
	virtual void CancelAttack() override;
	void SetCastbarPercentage();
	void RecastIfHolding();
	void MaintainAnim();
	void EnableMovement();
	FTimerHandle TimerHandle;
	FTimerHandle RecastTimerHandle;
	
	bool bReleaseReady = false;
	float HoldTimer;
	float AnimationTime = 0;

	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlayAnimationMontage(float PlayRate, FName StartSection = NAME_None, float MontagePosition = 0, bool bStartTimer = false);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_StopAnimationMontage();
};
