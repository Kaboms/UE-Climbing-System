// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbHandlers/HangClimbHandle.h"
#include "Components/ClimbingComponent.h"

void UHangClimbHandle::HandleMovement(FVector2D MoveDirection)
{
	if (IsValid(ClimbingSpline))
	{
		ACharacter* Character = ClimbingComponentBase->GetOwnerCharacter();

		FVector CharacterLocation = Character->GetActorLocation();

		FRotator ActorRotation = Character->GetActorRotation();
		const FRotator YawRotation(0, ActorRotation.Yaw, 0);

		float SplineInputKey = ClimbingSpline->FindInputKeyClosestToWorldLocation(Character->GetActorLocation());
		float ActorDistance = ClimbingSpline->GetDistanceAlongSplineAtSplineInputKey(SplineInputKey);

		FVector StartLocation = ClimbingSpline->GetLocationAtDistanceAlongSpline(ActorDistance, ESplineCoordinateSpace::Type::World);

		FRotator SplineRotation = ClimbingSpline->GetRotationAtSplineInputKey(SplineInputKey, ESplineCoordinateSpace::Type::World);

		int32 Side = (CharacterLocation - StartLocation).Dot(SplineRotation.Vector()) > 0 ? 1 : -1;
		float MoveRightDirection = (MovementDistance * MoveDirection.X) * Side;

		FVector NewLocation = ClimbingSpline->GetLocationAtDistanceAlongSpline(ActorDistance + (MoveRightDirection), ESplineCoordinateSpace::Type::World);

		FVector SplineMoveDirection = NewLocation - StartLocation;
		if (SplineMoveDirection.Length() > MinDistanceToSides && !SplineMoveDirection.IsNearlyZero(0.1))
		{
			SplineMoveDirection.Normalize();
			Character->AddMovementInput(SplineMoveDirection, ClimbingSpeedScale);

			ActorRotation.Yaw = SplineRotation.Yaw - 90 * Side;

			ClimbingComponentBase->TargetRotation = ActorRotation;

		}
		else
		{
			ClimbingComponentBase->StopMovement();
		}
	}
}

void UHangClimbHandle::GetHangPositionAndRotation(USplineComponent* InClimbingSpline, FVector CharacterLocation, FVector& Position, FRotator& Rotation)
{
	if (IsValid(InClimbingSpline))
	{
		float Key = InClimbingSpline->FindInputKeyClosestToWorldLocation(CharacterLocation);
		Rotation = InClimbingSpline->GetRotationAtSplineInputKey(Key, ESplineCoordinateSpace::Type::World);

		FRotator TempRotation = Rotation;
		FVector SplineLocation = InClimbingSpline->GetLocationAtSplineInputKey(Key, ESplineCoordinateSpace::Type::World);

		int32 Side = (CharacterLocation - SplineLocation).Dot(Rotation.Vector()) > 0 ? 1 : -1;

		TempRotation.Yaw += 90 * Side;

		Position = SplineLocation + (TempRotation.RotateVector(FVector::ForwardVector) * DistanceToCharacter);

		// Rotate player face to spline
		Rotation.Yaw -= 90 * Side;
	}
}
