// Richard Hill

#pragma once

#include "Components/ActorComponent.h"
#include "ManaComponent.generated.h"

class UHealthWidget;

UCLASS(meta=(BlueprintSpawnableComponent))
class UManaComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UManaComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mana")
	float CurrentMana = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mana")
	float BaseMaxMana = 100;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mana")
	float MaxMana = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mana")
	float IntelligenceManaMultiplier = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mana")
	float ManaGainPerSec = 1;

	UFUNCTION(BlueprintCallable)
	void UseMana(float ManaCost);
	
	UFUNCTION(BlueprintCallable)
	void GainMana(int IncomingMana);

	UFUNCTION(BlueprintCallable)
	bool CheckMana(float ManaCost);

	UFUNCTION()
	void IncreaseIntelligence(float Intelligence, int PlayerLevel);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	AActor* Owner;

	void GainManaPerSec(float DeltaTime);
};
