// By Davydov Mikhail

#include "Components/ClimbingComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "ClimbHandlers/ClimbHandlerBase.h"

// Sets default values for this component's properties
UClimbingComponent::UClimbingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UClimbingComponent::PostLoad()
{
	OwnerCharacter = Cast<ACharacter>(GetOwner());

	Super::PostLoad();
}

// Called when the game starts
void UClimbingComponent::BeginPlay()
{
	Super::BeginPlay();

	InitClimbHandlers();
}

void UClimbingComponent::InitClimbHandlers()
{
	for (auto ClimbHandlerIt : ClimbHandlers)
	{
		ClimbHandlerIt.Value->ClimbingComponentBase = this;
		ClimbHandlerIt.Value->Init();
	}
}

void UClimbingComponent::SmoothRotation()
{
	if (SmoothRotationInProgress)
	{
		FQuat AQuat(OwnerCharacter->GetActorRotation());
		FQuat BQuat(TargetRotation);

		OwnerCharacter->SetActorRotation(FQuat::Slerp(AQuat, BQuat, RotationSmoothAlpha).Rotator());
		
		SmoothRotationInProgress = !OwnerCharacter->GetActorRotation().Equals(TargetRotation, 0.1f);
	}
}

void UClimbingComponent::SmoothLocation()
{
	if (SmoothLocationInProgress)
	{
		if (!OwnerCharacter->GetActorLocation().Equals(TargetLocation, 2.0f))
		{
			OwnerCharacter->SetActorLocation(FMath::Lerp(OwnerCharacter->GetActorLocation(), TargetLocation, LocationSmoothAlpha));
		}
		else
		{
			SmoothLocationInProgress = false;
		}
	}
}

// Called every frame
void UClimbingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentClimbingType != EClimbingType::None)
	{
		SmoothRotation();
		SmoothLocation();
	}
}

void UClimbingComponent::EnableClimbingMode(EClimbingType ClimbingType)
{
	CurrentClimbingType = ClimbingType;

	OnClimbingTypeChanged.Broadcast(CurrentClimbingType);

	if (UClimbHandlerBase** ClimbHandlerPtr = ClimbHandlers.Find(CurrentClimbingType))
	{
		CurrentClimbHandler = (*ClimbHandlerPtr);
	}

	if (ControlCharacterMovement)
	{
		UCharacterMovementComponent* CharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
		CharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Flying);
		CharacterMovementComponent->StopMovementImmediately();
	}

	ReceiveEnableClimbingMode(ClimbingType);
}

void UClimbingComponent::DisableClimbingMode()
{
	CurrentClimbingType = EClimbingType::None;
	OnClimbingTypeChanged.Broadcast(CurrentClimbingType);

	SmoothRotationInProgress = false;
	SmoothLocationInProgress = false;

	if (IsValid(CurrentClimbHandler))
	{
		CurrentClimbHandler->EndClimb();
		CurrentClimbHandler = nullptr;
	}

	if (ControlCharacterMovement)
	{
		UCharacterMovementComponent* CharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
		CharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
	}
	ReceiveDisableClimbingMode();
}


FVector UClimbingComponent::GetPositionToEdgeWithOffset(float EdgeZPos, float Offset)
{
	FVector ActorLocation = OwnerCharacter->GetActorLocation();

	float CapsuleHalfHeight = OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	float ActorHeight = CapsuleHalfHeight * 2;

	float ActorZLocation = EdgeZPos - ActorHeight * Offset + CapsuleHalfHeight;

	return FVector(ActorLocation.X, ActorLocation.Y, ActorZLocation);
}

float UClimbingComponent::GetEdgeToCharacterRatio(float EdgeZPos)
{
	float CapsuleHalfHeight = OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector ActorLocation = OwnerCharacter->GetActorLocation();

	float ActorHeight = CapsuleHalfHeight * 2;
	float ActorBottom = ActorLocation.Z - CapsuleHalfHeight;

	return (EdgeZPos - ActorBottom) / ActorHeight;
}

bool UClimbingComponent::CanHandleMovement()
{
	return ControlCharacterMovement && HandleMoveClimbingTypes.Contains(CurrentClimbingType);
}

void UClimbingComponent::AddMovementInput(FVector2D MovementVector, float MovementScale)
{
	const FRotator Rotation = OwnerCharacter->GetActorRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	RightVector *= MovementVector.X;

	OwnerCharacter->AddMovementInput(RightVector, MovementScale);
	OwnerCharacter->AddMovementInput(OwnerCharacter->GetActorUpVector() * MovementVector.Y, MovementScale);
}

void UClimbingComponent::HandleMovement(FVector2D MovementVector)
{
	if (IsValid(CurrentClimbHandler))
	{
		CurrentClimbHandler->HandleMovement(MovementVector);
	}

	ReceiveHandleMovement(MovementVector);
}

void UClimbingComponent::StopMovement()
{
	OwnerCharacter->GetMovementComponent()->StopMovementImmediately();
}

void UClimbingComponent::SetTargetRotation(FRotator InTargetRotation)
{
	SmoothRotationInProgress = true;
	TargetRotation = InTargetRotation;
}

void UClimbingComponent::SetTargetLocation(FVector InTargetLocation)
{
	SmoothLocationInProgress = true;
	TargetLocation = InTargetLocation;
}
