// Richard Hill

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FreeLookCameraComponent.generated.h"

class USpringArmComponent;
class APlayerCharacter;
class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnForceCameraComplete);
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UFreeLookCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFreeLookCameraComponent();

	UFUNCTION(BlueprintCallable)
	USpringArmComponent* GetSpringArm() const { return SpringArm; }
	UFUNCTION(BlueprintCallable)
	UCameraComponent* GetCamera() const { return Camera; }
	void EnableCameraRotation(bool Enabled);
	void ScrollCamera(float Axis);
	void SetCameraDistance(float Axis);
	void SetCameraFoV(float Axis);
	void EnableFoVShift(bool Enable);
	UFUNCTION(BlueprintCallable)
	void SetCanMoveCamera(bool CanMoveCamera);
	UFUNCTION(BlueprintCallable)
	void ForceCameraToLocation(FRotator Rotation, float ArmLength, float FoV);
	UFUNCTION(BlueprintCallable)
	void ResetCameraLocation();
	UFUNCTION(BlueprintPure)
	bool GetCanMoveCamera() const { return bCanControlCamera; }
	UFUNCTION(BlueprintCallable)
	void SetCameraRotationSpeed(float SpeedAlpha);
	UFUNCTION(BlueprintCallable)
	float GetCameraRotationSpeed();

	UPROPERTY(BlueprintAssignable)
	FOnForceCameraComplete OnForceCameraComplete;

	UPROPERTY(BlueprintReadOnly)
	float CachedArmLength;
	UPROPERTY(BlueprintReadOnly)
	float CachedFoV;
	UPROPERTY(BlueprintReadOnly)
	FRotator CachedTargetRotation;
	UPROPERTY()
	bool EnableForceMoveCamera = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ForceMoveCameraTime = 1;
	float MoveCameraTimer = 0;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	USpringArmComponent* SpringArm;
	UPROPERTY()
	UCameraComponent* Camera;
	UPROPERTY()
	APlayerCharacter* PlayerCharacter;

	UPROPERTY(EditAnywhere, Category = CameraSettings)
	float CameraRotationSpeed = 1;
	UPROPERTY(EditAnywhere, Category = CameraSettings)
	float DistanceScrollSpeed = 500;
	UPROPERTY(EditAnywhere, Category = CameraSettings)
	float FoVScrollSpeed = 4;
	UPROPERTY(EditAnywhere, Category = CameraSettings)
	float NewArmLength = 2500;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraSettings)
	float MinArmLength = 1000;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraSettings)
	float MaxArmLength = 5000;
	UPROPERTY(EditAnywhere, Category = CameraSettings)
	float CameraDistanceInterpSpeed = 10;
	UPROPERTY(EditAnywhere, Category = CameraSettings)
	float NewFoV = 80;
	UPROPERTY(EditAnywhere, Category = CameraSettings)
	float MinFoV = 60;
	UPROPERTY(EditAnywhere, Category = CameraSettings)
	float MaxFoV = 90;
	UPROPERTY(EditAnywhere, Category = CameraSettings)
	float FoVInterpSpeed = 10;
	UPROPERTY(EditAnywhere, Category = CameraSettings)
	double MaxCameraPitchRotation = -84;
	UPROPERTY(EditAnywhere, Category = CameraSettings)
	double MinCameraPitchRotation = -10;
	UPROPERTY(EditAnywhere, Category = CameraSettings)
	float StartPitch = 75;
	UPROPERTY(EditAnywhere, Category = CameraSettings)
	float StartYaw = 75;
	UPROPERTY(EditAnywhere, Category = CameraSettings)
	float ForceMoveInterpStep = 0;
	UPROPERTY(EditAnywhere, Category = CameraSettings)
	FVector2D MinMaxCameraRotationSpeed = FVector2D(1, 100);

	bool bForcingCameraLocation = false;
	bool bRotatingCamera = false;
	bool bHoldingShift = false;
	bool bCanControlCamera = true;
	bool bReturningCameraToPrevPos = false;
	FRotator TargetRotation;

	void UpdateCameraDistance(float DeltaTime);
	void UpdateCameraFoV(float DeltaTime);
	void RotateCamera(float DeltaTime);
	

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ForceMoveCameraOnTick(float DeltaTime);
};
