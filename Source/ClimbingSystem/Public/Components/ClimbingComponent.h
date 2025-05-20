// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"

#include "ClimbingComponent.generated.h"

class UClimbHandlerBase;

UENUM(BlueprintType)
enum class EClimbingType : uint8
{
	None,
	// Just climb up on Object
	ClimbUp,
	// Climb over obstacles while running
	Obstacle,
	// Climb (crawn) on Object surface like Spider Man
	FreeClimb,
	// Climb along the wall snuggled to it
	SnuggleClimb,
	// Shimmy by hands
	ShimmyClimb,
	Ladder
};

USTRUCT(BlueprintType)
struct FClimbimgInputConfig
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> MoveInputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> JumpInputAction = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClimbingTypeChanged, EClimbingType, NewClimbingType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSmoothRotationFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSmoothLocationFinished);

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

	virtual void SetupInputComponent();

	void OnMoveInput(const FInputActionValue& Value);

	void OnJumpInput();

	UFUNCTION(BlueprintCallable)
	void EnableClimbingMode(EClimbingType ClimbingType);

	UFUNCTION(BlueprintCallable)
	void DisableClimbingMode();

	// If the offset equal zero - function return position where character bottom position will equal to edge.
	// To get character center to edge - set offset to 0.5f
	UFUNCTION(BlueprintPure)
	FVector GetPositionToEdgeWithOffset(float EdgeZPos, float Offset);
	
	// Return the ratio of the edge to character height where 0 is character capsule component bottom and 1 is top
	UFUNCTION(BlueprintPure)
	float GetEdgeToCharacterRatio(float EdgeZPos);

	UFUNCTION(BlueprintCallable)
	bool CanHandleMovement();

	UFUNCTION(BlueprintCallable)
	void AddMovementInput(FVector2D MovementVector, float MovementScale);

	UFUNCTION(BlueprintCallable)
	void HandleMovement(FVector2D MovementVector);

	UFUNCTION(BlueprintCallable)
	void StopMovement();

	UFUNCTION(BlueprintCallable)
	void SetTargetRotation(FRotator TargetRotation);

	UFUNCTION(BlueprintCallable)
	void SetTargetLocation(FVector TargetPosition);

	FORCEINLINE EClimbingType GetCurrentClimbingType() const { return CurrentClimbingType; }

	ACharacter* GetOwnerCharacter() { return OwnerCharacter; }

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void GetEdge(const FHitResult& Hit, FHitResult& OutHit, bool& EdgeIsSameActor);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void GetActorForwardTrace(double TraceDistance, float AngleDeg, FVector RotationAxis, FVector2D Offset, FVector& TraceStart, FVector& TraceEnd);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	FVector GetMoveTrace(FVector2D MoveDirection, FVector2D Distance);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool CheckTraceTimeout();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HandleHit(FHitResult HitResult, EClimbingType& SurfaceClimbingType, bool& Result);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void TraceForClimbing(bool& Result);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HandleClimbingType(FHitResult HitResult, EClimbingType ClimbingType, bool& Result);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ForceCheckClimbing(bool& Result);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitClimbHandlers();

	UFUNCTION(BlueprintCallable)
	void SmoothRotation(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void SmoothLocation(float DeltaTime);

	UFUNCTION(BlueprintPure)
	bool SmoothTranslationInProgress();

	UFUNCTION(BlueprintImplementableEvent, Meta=(DisplayName = "Handle Movement"))
	void ReceiveHandleMovement(FVector2D MovementVector);

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Enable Climbing Mode")
	void ReceiveEnableClimbingMode(EClimbingType ClimbingType);
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Disable Climbing Mode")
	void ReceiveDisableClimbingMode();

public:
	// Character climbing movement controlled by this component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ControlCharacterMovement = true;

	// Should be setted by OwnerCharacter
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CheckForClimbing = false;

	UPROPERTY(EditAnywhere, Category = "Smooth")
	float RotationSmoothSpeed = 10;

	float RotationSmoothAlpha = 0.0f;

	// TODO Smooth move to location broke climb up
	UPROPERTY(EditAnywhere, Category = "Smooth")
	float LocationSmoothSpeed = 10;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Climb Trace")
	TEnumAsByte<ETraceTypeQuery> TraceChannel;

	UPROPERTY(BlueprintAssignable)
	FClimbingTypeChanged OnClimbingTypeChanged;

	UPROPERTY(BlueprintAssignable)
	FSmoothRotationFinished OnSmoothRotationFinished;

	UPROPERTY(BlueprintAssignable)
	FSmoothLocationFinished OnSmoothLocationFinished;

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(BlueprintReadWrite)
	EClimbingType CurrentClimbingType;

	UPROPERTY(EditAnywhere)
	TSet<EClimbingType> HandleMoveClimbingTypes;

	bool ClimbingTypeHandleMovement = false;

	UPROPERTY(BlueprintReadWrite)
	bool ClimbingInTimeout = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
	TMap<EClimbingType, UClimbHandlerBase*> ClimbHandlers;

	UPROPERTY(BlueprintReadWrite)
	UClimbHandlerBase* CurrentClimbHandler;

	FRotator TargetRotation;
	bool SmoothRotationInProgress = false;

	FVector TargetLocation;
	bool SmoothLocationInProgress = false;

	UPROPERTY(EditAnywhere, Category = "Input")
	FClimbimgInputConfig ClimbimgInputConfig;
};
