// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ClimbHandlers/ClimbHandlerBase.h"
#include "ClimbUpHandler.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class CLIMBINGSYSTEM_API UClimbUpHandler : public UClimbHandlerBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void FindAndStartClimbUp(float EdgeZPos);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ClimbUp(UAnimMontage* AnimMontage);

public:
	// List of montages mapped to "Edge To Character Ratio"
	// Montage will play if edge ratio character to is nealy (ClimbUpErrorTolerance) equal to map key
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<float, UAnimMontage*> ClimbUpMontages;

	// On Climb Up edge to character ratio error tolerance
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ClimbUpErrorTolerance = 0.1f;
};
