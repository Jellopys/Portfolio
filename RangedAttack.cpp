// Richard Hill


#include "RangedAttack.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "SchoolTesting/MagicSpells/Projectiles/ForwardProjectile.h"
#include "SchoolTesting/Player/PlayerCharacter.h"

// Sets default values
ARangedAttack::ARangedAttack()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

float ARangedAttack::GetCurrentDrawTimeProgress()
{
	if (bFullyDrawn) return 1;
	
	if (GetWorldTimerManager().TimerExists(TimerHandle))
	{
		return GetWorldTimerManager().GetTimerElapsed(TimerHandle) / (BowDrawTime - FullyChargedTimeBuffer);
	}

	return 0;
}

void ARangedAttack::SetExtraArrowsToSpawn(int ExtraArrows)
{
	ExtraArrowsToSpawn += ExtraArrows;
}

void ARangedAttack::SetArrowBaseDamage(float NewBaseDamage)
{
	ArrowBaseDamage += NewBaseDamage;
}

void ARangedAttack::SetArrowPierceAmount(int NewPierceAmount)
{
	ArrowPierceAmount += NewPierceAmount;
}

// Called when the game starts or when spawned
void ARangedAttack::BeginPlay()
{
	Super::BeginPlay();
}

void ARangedAttack::Activate()
{
	if (HasAuthority())
	{
		Super::Activate();
	}
}

void ARangedAttack::Multi_Activate_Implementation()
{
	Super::Multi_Activate_Implementation();
}

void ARangedAttack::Server_Activate_Implementation()
{
	Super::Server_Activate_Implementation();

	bHolding = true;

	if (GetIsOnCooldown())
	{
		GetWorld()->GetTimerManager().SetTimer(RecastTimerHandle, this, &ARangedAttack::Activate, Cooldown - CooldownTimer + 0.01f, false);
	}
	
	else if (!GetIsOnCooldown())
	{
		OnCast();
		
		float const AtkSpeed = PlayerCharacter->StatsComponent->CharacterStats.AtkSpeed;
		float const PlayRate = AtkSpeed + AttackSpeedIncrease;
		BowDrawTime = PlayerCharacter->PlayAnimationMontage(BowAttackMontage, PlayRate, "Draw");
		Multi_PlayAnimationMontage(PlayRate, "Draw", 0, true);

		PlayerCharacter->SetIsDoingAction(true);
		PlayerCharacter->SetIsCasting(true);
		PlayerCharacter->SetNewMovementSpeed(CastingMovementSpeed);
	}
}

void ARangedAttack::Release()
{
	if (HasAuthority())
	{
		Super::Release();
	}
}

void ARangedAttack::Multi_Release_Implementation()
{
	Super::Multi_Release_Implementation();
}

void ARangedAttack::Server_Release_Implementation()
{
	Super::Server_Release_Implementation();
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(RecastTimerHandle);
	
	if (bHolding)
	{
		if (bFullyDrawn)
		{
			UseMana();

			for (int i = -ExtraArrowsToSpawn; i <= ExtraArrowsToSpawn; i++)
			{
				FRotator Angle = FRotator(0,i * AngleDelta, 0);
				FRotator AngledRotation = PlayerCharacter->GetProjectileSpawnRotation(RequiredWeaponType) + Angle;

				SpawnArrow(PlayerCharacter->FirePos->GetComponentLocation(), AngledRotation); // Spawns a pooled arrow from blueprint
			}
			
			float const AtkSpeed = PlayerCharacter->StatsComponent->CharacterStats.AtkSpeed;
			float const PlayRate = AtkSpeed + AttackSpeedIncrease;
			Multi_PlayAnimationMontage(PlayRate, "Release");
		}
		else
		{
			float const MontagePosition = PlayerCharacter->GetMontagePosition(BowAttackMontage);
			if (MontagePosition >= BowDrawTime)
			{
				float const AtkSpeed = PlayerCharacter->StatsComponent->CharacterStats.AtkSpeed;
				float const PlayRate = AtkSpeed + AttackSpeedIncrease;
				Multi_PlayAnimationMontage(-PlayRate, "Draw", MontagePosition);
			}
			else
			{
				float const AtkSpeed = PlayerCharacter->StatsComponent->CharacterStats.AtkSpeed;
				float const PlayRate = AtkSpeed + AttackSpeedIncrease;
				Multi_PlayAnimationMontage(-PlayRate, NAME_None, MontagePosition);
			}
		}	
		EnableMovement();
		OnRelease();
	}
}

void ARangedAttack::CancelAttack()
{
	Super::CancelAttack();
}

void ARangedAttack::Multi_Cancel_Implementation()
{
	Super::Multi_Cancel_Implementation();
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(RecastTimerHandle);
	PlayerCharacter->SetNewMovementSpeed(400);
	bFullyDrawn = false;
	bHolding = false;
	PlayerCharacter->SetIsCasting(false);
	PlayerCharacter->SetIsDoingAction(false);
	SetOnCooldown(true);
	PlayerCharacter->MagicComponent->SetCastbarPercentage(0);
}

void ARangedAttack::MaintainAnim()
{
	bFullyDrawn = true;
}

void ARangedAttack::EnableMovement()
{
	PlayerCharacter->SetNewMovementSpeed(400);
	bFullyDrawn = false;
	bHolding = false;
	PlayerCharacter->SetIsCasting(false);
	PlayerCharacter->SetIsDoingAction(false);
	SetOnCooldown(true);
	
}

void ARangedAttack::Multi_PlayAnimationMontage_Implementation(float PlayRate, FName StartSection, float MontagePosition, bool bStartTimer)
{
	BowDrawTime = PlayerCharacter->PlayAnimationMontage(BowAttackMontage, PlayRate, StartSection, MontagePosition);
	
	if (bStartTimer)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ARangedAttack::MaintainAnim, BowDrawTime - FullyChargedTimeBuffer, false);
	}
}

// Called every frame
void ARangedAttack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetCastbarPercentage();
}

void ARangedAttack::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ARangedAttack, bHolding);
	DOREPLIFETIME(ARangedAttack, bFullyDrawn);
}

void ARangedAttack::SetCastbarPercentage()
{
	if (!bHolding) return;
	
	if (bFullyDrawn)
	{
		PlayerCharacter->MagicComponent->SetCastbarPercentage(1);
		return;
	}
	
	PlayerCharacter->MagicComponent->SetCastbarPercentage(GetWorldTimerManager().GetTimerElapsed(TimerHandle) / (BowDrawTime - FullyChargedTimeBuffer));
}

