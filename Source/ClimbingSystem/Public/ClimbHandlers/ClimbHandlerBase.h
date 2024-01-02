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
	void Init();

	UFUNCTION(BlueprintCallable)
	void StartClimb(FHitResult HitResult);

	UFUNCTION(BlueprintCallable)
	FVector2D HandleMovement(FVector2D MoveDirection);

protected:
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Init")
	void ReceiveInit();

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Handle Movement")
	FVector2D ReceiveHandleMovement(FVector2D MoveDirection);

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Start Climb")
	void ReceiveStartClimb(FHitResult HitResult);

public:
	UPROPERTY(BlueprintReadOnly)
	UClimbingComponent* ClimbingComponentBase;
};
