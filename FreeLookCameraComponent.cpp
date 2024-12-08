// Richard Hill


#include "FreeLookCameraComponent.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "SchoolTesting/Player/PlayerCharacter.h"

UFreeLookCameraComponent::UFreeLookCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFreeLookCameraComponent::EnableCameraRotation(bool Enabled)
{
	bRotatingCamera = Enabled;
}

void UFreeLookCameraComponent::ScrollCamera(float Axis)
{
	if (bHoldingShift)
	{
		SetCameraFoV(Axis);
	}
	else
	{
		SetCameraDistance(Axis);
	}
}

void UFreeLookCameraComponent::SetCameraDistance(float Axis)
{
	if (!bCanControlCamera) return;
	NewArmLength = FMath::Clamp(NewArmLength -= Axis * DistanceScrollSpeed, MinArmLength, MaxArmLength);
}

void UFreeLookCameraComponent::SetCameraFoV(float Axis)
{
	if (!bCanControlCamera) return;
	NewFoV = FMath::Clamp(NewFoV -= Axis * FoVScrollSpeed, MinFoV, MaxFoV);
}

void UFreeLookCameraComponent::EnableFoVShift(bool Enable)
{
	bHoldingShift = Enable;
}

void UFreeLookCameraComponent::SetCanMoveCamera(bool CanMoveCamera)
{
	bCanControlCamera = CanMoveCamera;

	if (CanMoveCamera)
	{
		// Return camera to previous position
		bReturningCameraToPrevPos = true;
	}
}

void UFreeLookCameraComponent::ForceCameraToLocation(FRotator Rotation, float ArmLength, float FoV)
{
	// Cache current Camera Settings
	// CachedArmLength = NewArmLength;
	// CachedFoV  = NewFoV;
	// CachedTargetRotation = SpringArm->GetRelativeRotation();

	// Set New camera settings
	NewArmLength = ArmLength;
	NewFoV = FoV;
	TargetRotation = Rotation;

	bForcingCameraLocation = true;
}

void UFreeLookCameraComponent::ResetCameraLocation()
{
	// Set New camera settings
	NewArmLength = CachedArmLength;
	NewFoV = CachedFoV;
	TargetRotation = CachedTargetRotation;

	bForcingCameraLocation = true;
}

void UFreeLookCameraComponent::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	SpringArm = NewObject<USpringArmComponent>(this);
	SpringArm->SetupAttachment(GetOwner()->GetRootComponent());
	SpringArm->RegisterComponent();

	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 5;
	SpringArm->TargetArmLength = 3000;
	SpringArm->bInheritYaw = false;
	SpringArm->bDoCollisionTest = false;
	SpringArm->ProbeChannel = ECC_Camera;
	
	Camera = NewObject<UCameraComponent>(this);
	Camera->SetupAttachment(SpringArm);
	Camera->RegisterComponent();

	Camera->FieldOfView = NewFoV;
	
	const FRotator StartRotation = FRotator(StartPitch, StartYaw, 0);
	SpringArm->SetRelativeRotation(StartRotation);

	CachedArmLength = NewArmLength;
	CachedFoV  = NewFoV;
	CachedTargetRotation = SpringArm->GetRelativeRotation();
}

void UFreeLookCameraComponent::UpdateCameraDistance(float DeltaTime)
{
	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, NewArmLength, DeltaTime, CameraDistanceInterpSpeed);
}

void UFreeLookCameraComponent::UpdateCameraFoV(float DeltaTime)
{
	Camera->FieldOfView = FMath::FInterpTo(Camera->FieldOfView, NewFoV, DeltaTime, FoVInterpSpeed);
}

void UFreeLookCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	RotateCamera(DeltaTime);
	UpdateCameraDistance(DeltaTime);
	UpdateCameraFoV(DeltaTime);
	ForceMoveCameraOnTick(DeltaTime);
}

void UFreeLookCameraComponent::ForceMoveCameraOnTick(float DeltaTime)
{
	if (bReturningCameraToPrevPos || bForcingCameraLocation) 
	{
		if (bRotatingCamera)
		{
			bReturningCameraToPrevPos = false;
			// End the returnal early if the player decides to control the camera manually
		}
		
		const FRotator Rotation = FMath::RInterpTo(SpringArm->GetRelativeRotation(), TargetRotation, DeltaTime, ForceMoveInterpStep);
		SpringArm->SetRelativeRotation(Rotation);
		
		if (SpringArm->GetRelativeRotation().Equals(TargetRotation, 1))
		{
			bReturningCameraToPrevPos = false;
			bForcingCameraLocation = false;
			OnForceCameraComplete.Broadcast();
		}
		return;
	}
	
	if (!bCanControlCamera)
	{
		const FRotator Rotation = FMath::RInterpTo(SpringArm->GetRelativeRotation(), TargetRotation, DeltaTime, ForceMoveInterpStep);
		SpringArm->SetRelativeRotation(Rotation);
	}
}


void UFreeLookCameraComponent::RotateCamera(float DeltaTime)
{
	if (bCanControlCamera)
	{
		if (bReturningCameraToPrevPos) return;
		if (!bRotatingCamera) return;
		
		FVector2D MousePositionDelta = FVector2D(0, 0);
		GetWorld()->GetFirstPlayerController()->GetInputMouseDelta(MousePositionDelta.X, MousePositionDelta.Y);
		float Pitch = MousePositionDelta.Y * CameraRotationSpeed * DeltaTime;
		float Yaw = MousePositionDelta.X * CameraRotationSpeed * DeltaTime;
		
		if (SpringArm->GetRelativeRotation().Pitch < MaxCameraPitchRotation && Pitch < 0)
		{
			Pitch = 0;
		}
		else if (SpringArm->GetRelativeRotation().Pitch > MinCameraPitchRotation && Pitch > 0)
		{
			Pitch = 0;
		}
		
		TargetRotation = FRotator(Pitch, Yaw, 0);
		
		const FRotator Rotation = FMath::RInterpConstantTo(FRotator::ZeroRotator, TargetRotation, DeltaTime, 0);
		
		SpringArm->AddRelativeRotation(Rotation);
	}
}

void UFreeLookCameraComponent::SetCameraRotationSpeed(float SpeedAlpha)
{
	FVector2D const AlphaRange = FVector2D(0, 1);
	CameraRotationSpeed = FMath::GetMappedRangeValueClamped(AlphaRange, MinMaxCameraRotationSpeed, SpeedAlpha);
}

float UFreeLookCameraComponent::GetCameraRotationSpeed()
{
	return CameraRotationSpeed;
}
