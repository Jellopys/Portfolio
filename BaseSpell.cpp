// Richard Hill


#include "BaseSpell.h"

#include "ManaComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "SchoolTesting/Player/PlayerCharacter.h"
#include "SchoolTesting/Player/CustomComponents/Stats/DamageStatType.h"
#include "SchoolTesting/Player/CustomComponents/Stats/WeaponType.h"

ABaseSpell::ABaseSpell()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void ABaseSpell::BeginPlay()
{
	Super::BeginPlay();
	CooldownTimer = Cooldown;
}

void ABaseSpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CooldownTimer < Cooldown)
	{
		CooldownTimer += DeltaTime;
	}
}

void ABaseSpell::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseSpell, SpellOwner);
	// DOREPLIFETIME(ABaseSpell, PlayerCharacter);
}

void ABaseSpell::Activate()
{
	Server_Activate();
}

void ABaseSpell::Server_Activate_Implementation()
{
	
}

void ABaseSpell::Multi_Activate_Implementation()
{
	
}

void ABaseSpell::Release()
{
	Server_Release();
}

void ABaseSpell::Server_Release_Implementation()
{
	
}

void ABaseSpell::Multi_Release_Implementation()
{
}

void ABaseSpell::SetOnCooldown(bool InBool)
{
	// CooldownTimer = InBool ? 0 : Cooldown; // Fancy but less readable?
	if (InBool)
	{
		CooldownTimer = 0;
	}
	else
	{
		CooldownTimer = Cooldown;
	}
}

void ABaseSpell::SetSpellOwner_Implementation(AActor* InSpellOwner)
{
	SpellOwner = InSpellOwner;
	SetOwner(SpellOwner);

	// PlayerCharacter = Cast<APlayerCharacter>(SpellOwner);
}

void ABaseSpell::CancelAttack()
{
	OnCancel();
	Multi_Cancel();
}

void ABaseSpell::Multi_Cancel_Implementation()
{
}

float ABaseSpell::GetDamage(bool& IsCrit)
{
	float FinalDamage = BaseDamage;
	
	if (SpellOwner->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
	{
		float BonusPhysicalDamage = 0;
		float BonusMagicDamage = 0;
		float BonusRangedDamage = 0;
		UStatsComponent* StatsComponent = ICharacterInterface::Execute_GetStatsComponent(SpellOwner);
		
		for (const TPair<EDamageStatType, float> MapPair: DamageStatMap)
		{
			switch (MapPair.Key)
			{
			case EDamageStatType::PhysicalAttack:
				BonusPhysicalDamage = StatsComponent->CharacterStats.MeleeAttack * MapPair.Value;
				break;
			case EDamageStatType::MagicAttack:
				BonusMagicDamage = StatsComponent->CharacterStats.MagicAttack * MapPair.Value;
				break;
			case EDamageStatType::RangedAttack:
				BonusRangedDamage = StatsComponent->CharacterStats.RangedAttack * MapPair.Value;
				break;
				
				default:
					break;
			}
		}
		IsCrit = StatsComponent->GetIsCrit();
		float CritDmg = IsCrit ? 1.5f : 1;
		FinalDamage += BonusRangedDamage + BonusMagicDamage + BonusPhysicalDamage;
		FinalDamage *= CritDmg;
	}
	
	return FinalDamage;
}

bool ABaseSpell::GetIsCrit()
{
	return false;
}

bool ABaseSpell::GetIsOnCooldown()
{
	if (CooldownTimer >= Cooldown)
	{
		return false;
	}
	return true;
}

void ABaseSpell::UseMana()
{
	if (!ManaComponent) return;
	
	ManaComponent->UseMana(ManaCost);
}

void ABaseSpell::OnHit()
{
	
}


