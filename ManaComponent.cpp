// Richard Hill


#include "ManaComponent.h"

#include "Kismet/GameplayStatics.h"

UManaComponent::UManaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UManaComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();
	
	CurrentMana = MaxMana;
}

void UManaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	GainManaPerSec(DeltaTime);
}

void UManaComponent::GainManaPerSec(float DeltaTime)
{
	if (CurrentMana >= MaxMana) return;

	CurrentMana += ManaGainPerSec * DeltaTime;
}

void UManaComponent::UseMana(float ManaCost)
{
	FMath::Clamp(CurrentMana -= ManaCost, 0, MaxMana);
}

void UManaComponent::GainMana(int IncomingMana)
{
	CurrentMana = FMath::Clamp(CurrentMana += IncomingMana, 0, MaxMana);
}

bool UManaComponent::CheckMana(float ManaCost)
{
	if (ManaCost > CurrentMana)
	{
		return false;
	}
	return true;
}

void UManaComponent::IncreaseIntelligence(float Intelligence, int PlayerLevel)
{
	float const IncreaseAmount = Intelligence * IntelligenceManaMultiplier;
	MaxMana = BaseMaxMana + IncreaseAmount + (PlayerLevel * 10) + PlayerLevel;
	ManaGainPerSec = 1 + (Intelligence * 0.5f);
}
