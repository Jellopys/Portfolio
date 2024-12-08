// Richard Hill

#pragma once

#include "CoreMinimal.h"
#include "SchoolTesting/Player/CustomComponents/Magic/BaseSpell.h"
#include "SchoolTesting/Player/PlayerCharacter.h"
#include "RangedAttack.generated.h"

class AForwardProjectile;

UCLASS()
class ARangedAttack : public ABaseSpell
{
	GENERATED_BODY()

public:
	ARangedAttack();
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AForwardProjectile> ArrowProjectile;
	UPROPERTY(EditAnywhere)
	UAnimMontage* BowAttackMontage;
	UPROPERTY(EditAnywhere)
	float AttackSpeedIncrease = 0.3f;
	UPROPERTY(EditAnywhere)
	float FullyChargedTimeBuffer = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CastingMovementSpeed = 150;
	int ExtraArrowsToSpawn = 0;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnCast();

	UFUNCTION(BlueprintImplementableEvent)
	void OnRelease();

	UFUNCTION(BlueprintCallable)
	float GetCurrentDrawTimeProgress();

	UFUNCTION(BlueprintCallable)
	void SetExtraArrowsToSpawn(int ExtraArrows);
	UFUNCTION(BlueprintCallable)
	void SetArrowBaseDamage(float NewBaseDamage);
	UFUNCTION(BlueprintCallable)
	void SetArrowPierceAmount(int NewPierceAmount);
	

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bHolding = false;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Activate() override;
	virtual void Multi_Activate_Implementation() override;
	virtual void Server_Activate_Implementation() override;
	virtual void Release() override;
	virtual void Multi_Release_Implementation() override;
	virtual void Server_Release_Implementation() override;
	virtual void CancelAttack() override;
	virtual void Multi_Cancel_Implementation() override;
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnArrow(FVector SpawnLocation, FRotator SpawnRotation);
	void SetCastbarPercentage();
	void MaintainAnim();
	void EnableMovement();
	FTimerHandle TimerHandle;
	FTimerHandle RecastTimerHandle;

	UPROPERTY(Replicated)
	bool bFullyDrawn = false;
	float HoldTimer;
	float BowDrawTime = 0;
	float AngleDelta = 5;
	UPROPERTY(BlueprintReadWrite)
	float ArrowBaseDamage = 5;
	UPROPERTY(BlueprintReadWrite)
	int ArrowPierceAmount = 0;

	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlayAnimationMontage(float PlayRate, FName StartSection, float MontagePosition = 0, bool bStartTimer = false);
};
