// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ClimbHandlerBase.generated.h"

class UClimbingComponent;

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew, Meta = (ShowWorldContextPin))
class CLIMBINGSYSTEM_API UClimbHandlerBase : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void Init();

	UFUNCTION(BlueprintCallable)
	virtual void EndClimb();

	UFUNCTION(BlueprintCallable)
	virtual bool StartClimb(FHitResult HitResult);

	UFUNCTION(BlueprintCallable)
	virtual void HandleMovement(FVector2D MoveDirection);

protected:
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Init")
	void ReceiveInit();

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "End Climb")
	void ReceiveEndClimb();

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Handle Movement")
	void ReceiveHandleMovement(FVector2D MoveDirection);

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Start Climb")
	bool ReceiveStartClimb(FHitResult HitResult);

public:
	UPROPERTY(BlueprintReadOnly)
	UClimbingComponent* ClimbingComponentBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ClimbingSpeedScale = 1.0f;
};
