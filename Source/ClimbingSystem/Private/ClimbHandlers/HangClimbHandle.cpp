// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbHandlers/HangClimbHandle.h"
#include "Components/ClimbingComponent.h"

PRAGMA_DISABLE_OPTIMIZATION
void UHangClimbHandle::HandleMovement(FVector2D MoveDirection)
{
	if (IsValid(ClimbingSpline))
	{
		ACharacter* Character = ClimbingComponentBase->GetOwnerCharacter();
		float CharacterSplineKey = ClimbingSpline->FindInputKeyClosestToWorldLocation(Character->GetActorLocation());

		FVector2D SplineMoveDirection = MoveDirection;
		SplineMoveDirection *= Side;
		
		if (FMath::IsNearlyZero(SplineMoveDirection.X))
		{
			ClimbingComponentBase->StopMovement();
			return;
		}

		if ((SplineMoveDirection.X * CachedMoveDirection) > 0)
		{
			// Move direction not changed
			if ((SplineMoveDirection.X < 0 && CharacterSplineKey <= 0) || (SplineMoveDirection.X > 0 && CharacterSplineKey >= ClimbingSpline->GetNumberOfSplineSegments()))
			{
				ClimbingComponentBase->StopMovement();
				return;
			}

			//Character moved to target key - get next target position
			if ((SplineMoveDirection.X > 0 && CharacterSplineKey > NextLocationKey) || (SplineMoveDirection.X < 0 && CharacterSplineKey < NextLocationKey))
			{
				GetNextMoveDirection(MoveDirection, CharacterSplineKey);
			}
		}
		else
		{
			// Move direction changed - get new move direction
			ClimbingComponentBase->StopMovement();
			GetNextMoveDirection(MoveDirection, CharacterSplineKey);
		}

		if (!TargetSplineMoveDirection.IsNearlyZero(0.1))
		{
			Character->AddMovementInput(TargetSplineMoveDirection * ClimbingSpeedScale);

			FRotator Rotation;
			GetCharacterRotationAtSpline(CharacterSplineKey, Rotation);
			ClimbingComponentBase->SetTargetRotation(Rotation);
		}
		else
		{
			ClimbingComponentBase->StopMovement();
		}
	}
}

void UHangClimbHandle::EndClimb()
{
	Super::EndClimb();

	ClimbingSpline = nullptr;
}

void UHangClimbHandle::SetupSplineComponent(USplineComponent* InClimbingSpline)
{
	ClimbingSpline = InClimbingSpline;
	ACharacter* Character = ClimbingComponentBase->GetOwnerCharacter();
	FVector CharacterLocation = Character->GetActorLocation();

	float CharacterSplineKey = ClimbingSpline->FindInputKeyClosestToWorldLocation(CharacterLocation);
	FRotator TempRotation = ClimbingSpline->GetRotationAtSplineInputKey(CharacterSplineKey, ESplineCoordinateSpace::Type::World);
	TempRotation.Yaw += 90;

	FVector SplineLocation = ClimbingSpline->GetLocationAtSplineInputKey(CharacterSplineKey, ESplineCoordinateSpace::Type::World);

	FVector TempPosition = (CharacterLocation - SplineLocation);
	TempPosition.Normalize();

	FVector ForwardVector = TempRotation.Vector();

	Side = TempPosition.Dot(ForwardVector) > 0 ? 1 : -1;
	CachedMoveDirection = 0;
	NextLocationKey = 0;
}

void UHangClimbHandle::GetCharacterRotationAtSpline(float SplineKey, FRotator& Rotation)
{
	if (!IsValid(ClimbingSpline))
		return;

	Rotation = ClimbingSpline->GetRotationAtSplineInputKey(SplineKey, ESplineCoordinateSpace::Type::World);
	Rotation.Yaw -= 90 * Side;
}

void UHangClimbHandle::GetCharacterLocationAtSpline(float SplineKey, FVector& Location)
{
	if (!IsValid(ClimbingSpline))
		return;

	FVector SplineLocation = ClimbingSpline->GetLocationAtSplineInputKey(SplineKey, ESplineCoordinateSpace::Type::World);

	FRotator TempRotation = ClimbingSpline->GetRotationAtSplineInputKey(SplineKey, ESplineCoordinateSpace::Type::World);
	TempRotation.Yaw += 90 * Side;

	FVector ForwardVector = TempRotation.Vector();
	ForwardVector *= CharacterToSplineDistance;

	Location = SplineLocation + ForwardVector;
	Location.Z = ClimbingComponentBase->GetPositionToEdgeWithOffset(Location.Z, ZCharacterOffset).Z;
}

void UHangClimbHandle::GetNextMoveDirection(FVector2D MoveDirection, float CharacterSplineKey)
{
	CachedMoveDirection = (MoveDirection.X > 0 ? 1 : -1) * Side;

	NextLocationKey = FMath::Clamp<float>(CharacterSplineKey + 0.25f * CachedMoveDirection, 0, ClimbingSpline->GetNumberOfSplineSegments());
	FVector NextLocation;
	GetCharacterLocationAtSpline(NextLocationKey, NextLocation);

	TargetSplineMoveDirection = NextLocation - ClimbingComponentBase->GetOwnerCharacter()->GetActorLocation();
	TargetSplineMoveDirection.Normalize();
}
PRAGMA_ENABLE_OPTIMIZATION