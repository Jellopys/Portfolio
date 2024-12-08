// Richard Hill

#pragma once

#include "CoreMinimal.h"
#include "SchoolTesting/Player/CustomComponents/Magic/BaseSpell.h"
#include "Spell_DarkReaperSlash.generated.h"


UCLASS()
class ASpell_DarkReaperSlash : public ABaseSpell
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TravelSpeed = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CapsuleRadius = 42;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ForwardSpawnPos = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDrawDebug = false;

	UFUNCTION(BlueprintImplementableEvent)
	void PlaySoundOnHit();
protected:
	virtual void BeginPlay() override;
	virtual void Activate() override;
	void Swipe();

	

	bool bStopMoving = true;
};
