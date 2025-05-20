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
	UClimbUpHandler();

	UFUNCTION(BlueprintCallable)
	bool FindAndStartClimbUp(float EdgeZPos);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ClimbUp(UAnimMontage* AnimMontage);

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnSmoothLocationFinished();
	virtual void OnSmoothLocationFinished_Implementation();

public:
	// List of montages mapped to "Edge To Character Ratio" - 0 is bottom of character capsule and 1 is top
	// Montage will play if edge to ratio character is nealy (ClimbUpErrorTolerance) equal to map key
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<float, UAnimMontage*> ClimbUpMontages;

	// On Climb Up edge to character ratio error tolerance
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ClimbUpErrorTolerance = 0.1f;

protected:
	UPROPERTY(BlueprintReadOnly)
	UAnimMontage* CurrentClimbUpMontage;
};
