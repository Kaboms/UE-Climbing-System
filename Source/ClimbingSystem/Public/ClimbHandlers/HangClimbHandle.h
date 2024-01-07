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

	virtual void EndClimb() override;

	UFUNCTION(BlueprintCallable)
	void SetupSplineComponent(USplineComponent* InClimbingSpline);

	UFUNCTION(BlueprintCallable)
	void GetCharacterRotationAtSpline(float SplineKey, FRotator& Rotation);

	UFUNCTION(BlueprintCallable)
	void GetCharacterLocationAtSpline(float SplineKey, FVector& Location);

	void GetNextMoveDirection(FVector2D MoveDirection, float CharacterSplineKey);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CharacterToSplineDistance = 50;

	// 0 is character capsule bottom and 1 is top
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ZCharacterOffset = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinMovementDistance = 40;

protected:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<USplineComponent> ClimbingSpline;

	UPROPERTY(BlueprintReadWrite)
	int32 Side;

	float NextLocationKey = 0.0f;

	FVector TargetSplineMoveDirection;

	int32 CachedMoveDirection = 0;
};
