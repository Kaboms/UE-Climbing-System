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

	bAutoActivate = true;
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
	SetupInputComponent();
}

void UClimbingComponent::InitClimbHandlers()
{
	for (auto ClimbHandlerIt : ClimbHandlers)
	{
		ClimbHandlerIt.Value->ClimbingComponentBase = this;
		ClimbHandlerIt.Value->Init();
	}
}

void UClimbingComponent::SmoothRotation(float DeltaTime)
{
	if (SmoothRotationInProgress)
	{
		FQuat AQuat(OwnerCharacter->GetActorRotation());
		FQuat BQuat(TargetRotation);

		RotationSmoothAlpha = FMath::FInterpTo(RotationSmoothAlpha, 1.0f, DeltaTime, RotationSmoothSpeed);

		OwnerCharacter->SetActorRotation(FQuat::Slerp(AQuat, BQuat, RotationSmoothAlpha));
		
		SmoothRotationInProgress = !OwnerCharacter->GetActorRotation().Equals(TargetRotation, 0.1f);
	}
}

void UClimbingComponent::SmoothLocation(float DeltaTime)
{
	if (SmoothLocationInProgress)
	{
		if (!OwnerCharacter->GetActorLocation().Equals(TargetLocation, 2.0f))
		{
			OwnerCharacter->SetActorLocation(FMath::VInterpTo(OwnerCharacter->GetActorLocation(), TargetLocation, DeltaTime, LocationSmoothSpeed));
		}
		else
		{
			SmoothLocationInProgress = false;
		}
	}
}

bool UClimbingComponent::SmoothTranslationInProgress()
{
	return SmoothLocationInProgress || SmoothLocationInProgress;
}

// Called every frame
void UClimbingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsActive())
		return;

	if (CurrentClimbingType != EClimbingType::None)
	{
		SmoothRotation(DeltaTime);
		SmoothLocation(DeltaTime);
	}
}

void UClimbingComponent::SetupInputComponent()
{
	if (!IsValid(OwnerCharacter->InputComponent))
		return;

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(OwnerCharacter->InputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(ClimbimgInputConfig.JumpInputAction, ETriggerEvent::Triggered, this, &UClimbingComponent::OnJumpInput);

		//Moving
		EnhancedInputComponent->BindAction(ClimbimgInputConfig.MoveInputAction, ETriggerEvent::Triggered, this, &UClimbingComponent::OnMoveInput);
		EnhancedInputComponent->BindAction(ClimbimgInputConfig.MoveInputAction, ETriggerEvent::Completed, this, &UClimbingComponent::StopMovement);
	}
}

void UClimbingComponent::OnMoveInput(const FInputActionValue& Value)
{
	if (CanHandleMovement() && !SmoothTranslationInProgress())
	{
		// Input is a Vector2D
		FVector2D MovementVector = Value.Get<FVector2D>();

		HandleMovement(MovementVector);
	}
}

void UClimbingComponent::OnJumpInput()
{
	if (CanHandleMovement())
	{
		DisableClimbingMode();
	}
}

void UClimbingComponent::EnableClimbingMode(EClimbingType ClimbingType)
{
	if (!IsActive())
		return;

	CurrentClimbingType = ClimbingType;

	OnClimbingTypeChanged.Broadcast(CurrentClimbingType);

	if (UClimbHandlerBase** ClimbHandlerPtr = ClimbHandlers.Find(CurrentClimbingType))
	{
		CurrentClimbHandler = (*ClimbHandlerPtr);
	}
	else
	{
		checkNoEntry();
	}

	if (ControlCharacterMovement)
	{
		UCharacterMovementComponent* CharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
		CharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Flying);
		CharacterMovementComponent->StopMovementImmediately();

		ClimbingTypeHandleMovement = HandleMoveClimbingTypes.Contains(CurrentClimbingType);
	}

	ReceiveEnableClimbingMode(ClimbingType);
}

void UClimbingComponent::DisableClimbingMode()
{
	CurrentClimbingType = EClimbingType::None;
	OnClimbingTypeChanged.Broadcast(CurrentClimbingType);

	SmoothRotationInProgress = false;
	SmoothLocationInProgress = false;
	ClimbingTypeHandleMovement = false;

	CurrentClimbHandler->EndClimb();
	CurrentClimbHandler = nullptr;

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
	return IsActive() && ControlCharacterMovement && ClimbingTypeHandleMovement;
}

void UClimbingComponent::AddMovementInput(FVector2D MovementVector, float MovementScale)
{
	if (!IsActive())
		return;

	const FRotator Rotation = OwnerCharacter->GetActorRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	RightVector *= MovementVector.X;

	OwnerCharacter->AddMovementInput(RightVector, MovementScale);
	OwnerCharacter->AddMovementInput(OwnerCharacter->GetActorUpVector() * MovementVector.Y, MovementScale);
}

void UClimbingComponent::HandleMovement(FVector2D MovementVector)
{
	if (!IsActive())
		return;

	if (IsValid(CurrentClimbHandler))
	{
		CurrentClimbHandler->HandleMovement(MovementVector);
	}

	ReceiveHandleMovement(MovementVector);
}

void UClimbingComponent::StopMovement()
{
	if (CanHandleMovement())
	{
		OwnerCharacter->GetMovementComponent()->StopMovementImmediately();
	}
}

void UClimbingComponent::SetTargetRotation(FRotator InTargetRotation)
{
	if (RotationSmoothSpeed > 0)
	{
		SmoothRotationInProgress = true;
		RotationSmoothAlpha = 0.0f;
		TargetRotation = InTargetRotation;
	}
	else
	{
		OwnerCharacter->SetActorRotation(InTargetRotation);
	}
}

void UClimbingComponent::SetTargetLocation(FVector InTargetLocation)
{
	if (LocationSmoothSpeed > 0)
	{
		SmoothLocationInProgress = true;
		TargetLocation = InTargetLocation;
	}
	else
	{
		OwnerCharacter->SetActorLocation(InTargetLocation);
	}
}
