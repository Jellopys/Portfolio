// Richard Hill


#include "Spell_TwoHand_Melee_Attack.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "SchoolTesting/Player/PlayerCharacter.h"
#include "SchoolTesting/Player/CustomComponents/HealthComponent.h"

ASpell_TwoHand_Melee_Attack::ASpell_TwoHand_Melee_Attack()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASpell_TwoHand_Melee_Attack::UpgradeDamage(float ExtraDamage)
{
	BaseDamage += ExtraDamage;
}

void ASpell_TwoHand_Melee_Attack::UpgradeCleave(float ExtraRadius)
{
	CleaveRadius += ExtraRadius;
}

float ASpell_TwoHand_Melee_Attack::GetCurrentAttackTimeProgress()
{
	if (bReleaseReady) return 1;
	
	if (GetWorldTimerManager().TimerExists(TimerHandle))
	{
		return GetWorldTimerManager().GetTimerElapsed(TimerHandle) / (AnimationTime - FullyChargedTimeBuffer);
	}

	return 0;
}

void ASpell_TwoHand_Melee_Attack::BeginPlay()
{
	Super::BeginPlay();
}

void ASpell_TwoHand_Melee_Attack::Activate()
{
	if (HasAuthority())
	{
		Super::Activate();
	}
}

void ASpell_TwoHand_Melee_Attack::Release()
{
	if (HasAuthority())
	{
		Super::Release();
	}
}

void ASpell_TwoHand_Melee_Attack::CancelAttack()
{
	Super::CancelAttack();
}

void ASpell_TwoHand_Melee_Attack::RecastIfHolding()
{	
	if (bHolding == true)
	{
		OnCast();
		
		float const AtkSpeed = PlayerCharacter->StatsComponent->CharacterStats.AtkSpeed;
		float const PlayRate = AtkSpeed + AttackSpeedIncrease;
		Multi_PlayAnimationMontage(PlayRate, NAME_None, 0, true);

		return;
	}
	
	EnableMovement();
}

void ASpell_TwoHand_Melee_Attack::MaintainAnim()
{
	bReleaseReady = true;
}

void ASpell_TwoHand_Melee_Attack::EnableMovement()
{
	bReleaseReady = false;
	bHolding = false;
	PlayerCharacter->SetIsCasting(false);
	PlayerCharacter->SetIsDoingAction(false);
	SetOnCooldown(true);
}

void ASpell_TwoHand_Melee_Attack::Multi_StopAnimationMontage_Implementation()
{
	PlayerCharacter->StopAnimation(MeleeAttackMontage, 0.2f);
}

void ASpell_TwoHand_Melee_Attack::Multi_PlayAnimationMontage_Implementation(float PlayRate, FName StartSection,
                                                                            float MontagePosition, bool bStartTimer)
{
	AnimationTime = PlayerCharacter->PlayAnimationMontage(MeleeAttackMontage, PlayRate, StartSection, MontagePosition);
	
	if (bStartTimer)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASpell_TwoHand_Melee_Attack::RecastIfHolding, AnimationTime, false);
	}
}

void ASpell_TwoHand_Melee_Attack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASpell_TwoHand_Melee_Attack::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASpell_TwoHand_Melee_Attack, bHolding)
}

void ASpell_TwoHand_Melee_Attack::Server_Activate_Implementation()
{
	Super::Server_Activate_Implementation();

	if (GetIsOnCooldown())
	{
		GetWorld()->GetTimerManager().SetTimer(RecastTimerHandle, this, &ASpell_TwoHand_Melee_Attack::Activate, Cooldown - CooldownTimer + 0.01f, false);
	}
	else
	{
		OnCast();
		
		float const AtkSpeed = PlayerCharacter->StatsComponent->CharacterStats.AtkSpeed;
		float const PlayRate = AtkSpeed + AttackSpeedIncrease;
		Multi_PlayAnimationMontage(PlayRate, "Draw", 0, true);
		bHolding = true;
		
		PlayerCharacter->SetIsDoingAction(true);
		PlayerCharacter->SetIsCasting(true);
		PlayerCharacter->SetNewMovementSpeed(MovementSpeedDuringAttack);
	}
}

void ASpell_TwoHand_Melee_Attack::Server_Release_Implementation()
{
	Super::Server_Release_Implementation();

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(RecastTimerHandle);
	
	if (bHolding)
	{
		Multi_StopAnimationMontage();
		if (PlayerCharacter->HealthComponent->GetIsDead()) return;
		
		EnableMovement();
		PlayerCharacter->SetDefaultMovementSpeed();
		OnRelease();
	}
}

void ASpell_TwoHand_Melee_Attack::Multi_Activate_Implementation()
{
	Super::Multi_Activate_Implementation();
}

void ASpell_TwoHand_Melee_Attack::Multi_Release_Implementation()
{
	Super::Multi_Release_Implementation();
}

void ASpell_TwoHand_Melee_Attack::Multi_Cancel_Implementation()
{
	Super::Multi_Cancel_Implementation();

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(RecastTimerHandle);
	PlayerCharacter->StopAnimation(MeleeAttackMontage, 0.2f);
	PlayerCharacter->SetDefaultMovementSpeed();
	bHolding = false;
	PlayerCharacter->SetIsCasting(false);
	PlayerCharacter->SetIsDoingAction(false);
	SetOnCooldown(true);
}

void ASpell_TwoHand_Melee_Attack::SetCastbarPercentage()
{
	if (!bHolding) return;
	
	if (bReleaseReady)
	{
		PlayerCharacter->MagicComponent->SetCastbarPercentage(1);
		return;
	}
	
	PlayerCharacter->MagicComponent->SetCastbarPercentage(GetWorldTimerManager().GetTimerElapsed(TimerHandle) / (AnimationTime - FullyChargedTimeBuffer));
}

