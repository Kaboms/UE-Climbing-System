// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbHandlers/ShimmyClimbHandler.h"
#include "Components/ClimbingComponent.h"

void UShimmyClimbHandler::HandleMovement(FVector2D MoveDirection)
{
	if (IsValid(ClimbingSpline))
	{
		ACharacter* Character = ClimbingComponent->GetOwnerCharacter();
		float CharacterSplineKey = ClimbingSpline->FindInputKeyClosestToWorldLocation(Character->GetActorLocation());

		FVector2D SplineMoveDirection = MoveDirection;
		SplineMoveDirection *= Side;
		
		if (FMath::IsNearlyZero(SplineMoveDirection.X))
		{
			ClimbingComponent->StopMovement();
			return;
		}

		if ((SplineMoveDirection.X * CachedMoveDirection) > 0)
		{
			// Move direction not changed
			if ((SplineMoveDirection.X < 0 && CharacterSplineKey <= 0) || (SplineMoveDirection.X > 0 && CharacterSplineKey >= ClimbingSpline->GetNumberOfSplineSegments()))
			{
				ClimbingComponent->StopMovement();
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
			ClimbingComponent->StopMovement();
			GetNextMoveDirection(MoveDirection, CharacterSplineKey);
		}

		if (!TargetSplineMoveDirection.IsNearlyZero(0.1))
		{
			Character->AddMovementInput(TargetSplineMoveDirection * ClimbingSpeedScale);

			FRotator Rotation;
			GetCharacterRotationAtSpline(CharacterSplineKey, Rotation);
			ClimbingComponent->SetTargetRotation(Rotation);
		}
		else
		{
			ClimbingComponent->StopMovement();
		}
	}
}

void UShimmyClimbHandler::EndClimb()
{
	Super::EndClimb();

	ClimbingSpline = nullptr;
}

void UShimmyClimbHandler::SetupSplineComponent(USplineComponent* InClimbingSpline)
{
	ClimbingSpline = InClimbingSpline;
	ACharacter* Character = ClimbingComponent->GetOwnerCharacter();
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

void UShimmyClimbHandler::GetCharacterRotationAtSpline(float SplineKey, FRotator& Rotation)
{
	if (!IsValid(ClimbingSpline))
		return;

	Rotation = ClimbingSpline->GetRotationAtSplineInputKey(SplineKey, ESplineCoordinateSpace::Type::World);
	Rotation.Yaw -= 90 * Side;
}

void UShimmyClimbHandler::GetCharacterLocationAtSpline(float SplineKey, FVector& Location)
{
	if (!IsValid(ClimbingSpline))
		return;

	FVector SplineLocation = ClimbingSpline->GetLocationAtSplineInputKey(SplineKey, ESplineCoordinateSpace::Type::World);

	FRotator TempRotation = ClimbingSpline->GetRotationAtSplineInputKey(SplineKey, ESplineCoordinateSpace::Type::World);
	TempRotation.Yaw += 90 * Side;

	FVector ForwardVector = TempRotation.Vector();
	ForwardVector *= CharacterToSplineDistance;

	Location = SplineLocation + ForwardVector;
	Location.Z = ClimbingComponent->GetPositionToEdgeWithOffset(Location.Z, ZCharacterOffset).Z;
}

void UShimmyClimbHandler::GetNextMoveDirection(FVector2D MoveDirection, float CharacterSplineKey)
{
	CachedMoveDirection = (MoveDirection.X > 0 ? 1 : -1) * Side;

	NextLocationKey = FMath::Clamp<float>(CharacterSplineKey + 0.25f * CachedMoveDirection, 0, ClimbingSpline->GetNumberOfSplineSegments());
	FVector NextLocation;
	GetCharacterLocationAtSpline(NextLocationKey, NextLocation);

	TargetSplineMoveDirection = NextLocation - ClimbingComponent->GetOwnerCharacter()->GetActorLocation();
	TargetSplineMoveDirection.Normalize();
}
