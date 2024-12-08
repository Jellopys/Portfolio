
#include "MagicComponent.h"
#include "Net/UnrealNetwork.h"
#include "BaseSpell.h"
#include "SchoolTesting/Player/CustomComponents/Stats/WeaponType.h"

UMagicComponent::UMagicComponent()
{
	SetIsReplicated(true);
}

void UMagicComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (MovementSpellClass)
	{
		MovementSpell = GetWorld()->SpawnActor<ABaseSpell>(MovementSpellClass);
		MovementSpell->SetSpellOwner(GetOwner());
	}
	
	for (TPair<EWeaponType, TSubclassOf<ABaseSpell>> MapPair: BasicSpells)
	{
		auto* SpawnedSpell = GetWorld()->SpawnActor<ABaseSpell>(MapPair.Value);
		SpawnedSpell->SetSpellOwner(GetOwner());
		SpawnedBasicSpells.Add(MapPair.Key, SpawnedSpell);
	}	
}

void UMagicComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TickManaPerSec(DeltaTime);
}

void UMagicComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMagicComponent, CurrentSpell);
}

void UMagicComponent::TickManaPerSec(float DeltaTime)
{
	if (CurrentMana >= MaxMana) return;

	CurrentMana = FMath::Clamp(CurrentMana += ManaGainPerSecond * DeltaTime, 0, MaxMana);
}

void UMagicComponent::PrimaryFire(EWeaponType WeaponType)
{
	if (!SpawnedBasicSpells.Contains(WeaponType)) return;
		
	auto* UnCastedBasicSpell = SpawnedBasicSpells.Find(WeaponType); // BAD CAST, REFACTOR
	auto* BasicSpell = Cast<ABaseSpell>(*UnCastedBasicSpell);
	CurrentSpell = BasicSpell;
	
	if (BasicSpell)
	{
		BasicSpell->Activate();
	}
}

void UMagicComponent::PrimaryRelease()
{
	if (CurrentSpell)
	{
		bCanChangeSpell = false;
		CurrentSpell->Release();
	}
}

void UMagicComponent::SecondaryFire()
{
	if (SecondarySpell)
	{
		SecondarySpell->Activate();
	}
}

void UMagicComponent::ActivateMovementSpell()
{
	if (MovementSpell)
	{
		MovementSpell->Activate();
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("Movement spell is not valid"));
	}
}

void UMagicComponent::CancelAttacks()
{
	if (CurrentSpell)
	{
		CurrentSpell->CancelAttack();
	}
}

void UMagicComponent::ChangeSpell(int SpellBookIndex)
{
	if (bCanChangeSpell) return;
	if (SpellBookIndex > SpawnedSpells.Num()) return;
	if (!SpawnedSpells[SpellBookIndex - 1]) return;
	
	CurrentSpell = Cast<ABaseSpell>(SpawnedSpells[SpellBookIndex - 1]);
	OnChangeSpell.Broadcast(SpellBookIndex);
}

void UMagicComponent::SetCastbarPercentage(float Percentage)
{
	CastbarPercentage = Percentage;
}

void UMagicComponent::GainMana(int IncomingMana)
{
	CurrentMana = FMath::Clamp(CurrentMana += IncomingMana, 0, MaxMana);
}

bool UMagicComponent::UseMana(int ManaCost)
{
	if (ManaCost > CurrentMana) return false;

	CurrentMana = FMath::Clamp(CurrentMana -= ManaCost, 0, MaxMana);
	return true;
}




