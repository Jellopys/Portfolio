// Richard Hill


#include "Spell_DarkReaperSlash.h"

#include "Kismet/GameplayStatics.h"
#include "SchoolTesting/Enemy/BaseEnemy.h"
#include "SchoolTesting/Interfaces/HealthInterface.h"
#include "SchoolTesting/Player/CustomComponents/HealthComponent.h"
#include "SchoolTesting/Player/CustomComponents/Stats/StatsComponent.h"


void ASpell_DarkReaperSlash::BeginPlay()
{
	Super::BeginPlay();
}

void ASpell_DarkReaperSlash::Activate()
{
	Super::Activate();

	if (!SpellOwner) return;
	
	Swipe();
}

void ASpell_DarkReaperSlash::Swipe()
{
	TArray<FHitResult> HitResults;
	FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleRadius);
	FVector ForwardVector = SpellOwner->GetActorLocation() + SpellOwner->GetActorForwardVector() * ForwardSpawnPos;
	GetWorld()->SweepMultiByChannel(HitResults, ForwardVector,ForwardVector, FQuat::Identity, ECC_Pawn, CapsuleShape);

	if (bDrawDebug)
	{
		DrawDebugCapsule(GetWorld(), ForwardVector, CapsuleRadius, CapsuleRadius, FQuat::Identity, FColor::Red, false, 5);
	}

	for (auto Hit : HitResults)
	{
		if (!Hit.bBlockingHit) continue;
		if (!Hit.GetActor()) continue;
		if (!SpellOwner) continue;
		if (Hit.GetActor() == SpellOwner) continue;

		if (Hit.GetActor()->GetClass()->ImplementsInterface(UHealthInterface::StaticClass()))
		{
			// if they are NOT on the same team, deal damage
			if (IHealthInterface::Execute_GetTeam(Hit.GetActor()) != IHealthInterface::Execute_GetTeam(SpellOwner))
			{
				bool bIsCrit = false;
				IHealthInterface::Execute_TakeDamage(Hit.GetActor(), GetDamage(bIsCrit), bIsCrit, SpellOwner);
				PlaySoundOnHit();
				return;
			}
		}
	}
}

