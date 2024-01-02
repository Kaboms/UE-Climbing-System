// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClimbingComponent.generated.h"

UENUM(BlueprintType)
enum class EClimbingType : uint8
{
	None,
	ClimbUp, // Just climb up on Object
	Obstacle, // Climb over obstacles while running
	FreeClimb, // Climb (crawn) on Object surface like Spider Man
	SnuggleClimb, // Climb along the wall snuggled to it
	HangingOnHands,
	Ladder
};

USTRUCT(BlueprintType)
struct FClimbUpHandler
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinDistanceToEdge;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* ClimbUpMontage;
};

UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CLIMBINGSYSTEM_API UClimbingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UClimbingComponent();

	virtual void PostLoad() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void StartClimbUp(float EdgeZPos);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ClimbUp(UAnimMontage* AnimMontage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void DisableClimbingMode();

	// If the offset equal zero - function return position where character bottom position will equal to edge.
	// To get character center to edge - set offset to 0.5f
	UFUNCTION(BlueprintCallable)
	FVector GetPositionToEdgeWithOffset(float EdgeZPos, float Offset);
	
	// Return the ratio of the edge to character height where 0 is character capsule component bottom and 1 is top
	UFUNCTION(BlueprintPure)
	float GetEdgeToCharacterRatio(float EdgeZPos);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void  HandleFreeClimbMovement(UPARAM(ref) FVector2D& MoveDirection);

	FORCEINLINE EClimbingType GetCurrentClimbingType() const { return CurrentClimbingType; }

	UFUNCTION(BlueprintCallable)
	bool CanHandleMovement();

	UFUNCTION(BlueprintCallable)
	void HandleMovement(FVector2D MovementVector);

	void StopMovement();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Meta=(DisplayName = "Handle Movement"))
	void ReceiveHandleMovement(FVector2D MovementVector);

public:
	// Character climbing movement controlled by this component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ControlCharacterMovement = true;

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class ACharacter> OwnerCharacter;

	// List of montages mapped to "Edge To Character Ratio"
	// Montage will play if character ratio to edge is nealy (ClimbUpErrorTolerance) equal to map key
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<float, UAnimMontage*> ClimbUpMontages;

	// On Climb Up edge to character ratio error tolerance
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ClimbUpErrorTolerance = 0.1f;

	// Character movement speed may be different when they walked and climbing
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ClimbingSpeedScale = 1.0f;

	UPROPERTY(BlueprintReadWrite)
	EClimbingType CurrentClimbingType;

	UPROPERTY(BlueprintReadWrite)
	bool ClimbingInTimeout = false;

};
