// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ClimbHandlers/ClimbHandlerBase.h"
#include "Components/SplineComponent.h"
#include "HangClimbHandle.generated.h"

/**
 * 
 */
UCLASS()
class CLIMBINGSYSTEM_API UHangClimbHandle : public UClimbHandlerBase
{
	GENERATED_BODY()

public:
	virtual void HandleMovement(FVector2D MoveDirection) override;

	UFUNCTION(BlueprintCallable)
	void GetHangPositionAndRotation(USplineComponent* InClimbingSpline, FVector CharacterLocation, FVector& Position, FRotator& Rotation);

public:
	// Clamp character between start and end point of the spline
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MinDistanceToSides = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DistanceToCharacter= 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MovementDistance = 100;

protected:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<USplineComponent> ClimbingSpline;

	float CurrentSplinePosition;

};
