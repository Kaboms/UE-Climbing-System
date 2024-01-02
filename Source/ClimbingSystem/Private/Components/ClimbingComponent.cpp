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

// Called every frame
void UClimbingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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
	return ControlCharacterMovement && CurrentClimbingType != EClimbingType::None && CurrentClimbingType != EClimbingType::ClimbUp;
}

void UClimbingComponent::HandleMovement(FVector2D MovementVector)
{
	if (UClimbHandlerBase** ClimbHandlerPtr = ClimbHandlers.Find(CurrentClimbingType))
	{
		MovementVector = (*ClimbHandlerPtr)->HandleMovement(MovementVector);
	}

	if (MovementVector.Length() != 0)
	{
		const FRotator Rotation = OwnerCharacter->GetActorRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		RightVector *= MovementVector.X;

		OwnerCharacter->AddMovementInput(RightVector, ClimbingSpeedScale);
		OwnerCharacter->AddMovementInput(OwnerCharacter->GetActorUpVector() * MovementVector.Y, ClimbingSpeedScale);
	}
	else
	{
		StopMovement();
	}

	ReceiveHandleMovement(MovementVector);
}

void UClimbingComponent::StopMovement()
{
	OwnerCharacter->GetMovementComponent()->StopMovementImmediately();
}
