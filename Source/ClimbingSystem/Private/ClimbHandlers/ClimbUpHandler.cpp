// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbHandlers/ClimbUpHandler.h"
#include "Components/ClimbingComponent.h"
#include "GameFramework/Character.h"

UClimbUpHandler::UClimbUpHandler() : Super()
{
	InterruptByJump = false;
}

bool UClimbUpHandler::FindAndStartClimbUp(float EdgeZPos)
{
	float EdgeToCharacterCoof = ClimbingComponent->GetEdgeToCharacterRatio(EdgeZPos);

	for (const auto& ClimbUpMontage : ClimbUpMontages)
	{
		if (FMath::IsNearlyEqual(EdgeToCharacterCoof, ClimbUpMontage.Key, ClimbUpErrorTolerance))
		{
			CurrentClimbUpMontage = ClimbUpMontage.Value;

			ClimbingComponent->OnSmoothLocationFinished.AddDynamic(this, &ThisClass::OnSmoothLocationFinished);

			ClimbUp(CurrentClimbUpMontage);

			ClimbingComponent->SetTargetLocation(ClimbingComponent->GetPositionToEdgeWithOffset(EdgeZPos, ClimbUpMontage.Key));

			return true;
		}
	}

	return false;
}

void UClimbUpHandler::OnSmoothLocationFinished_Implementation()
{
	ClimbingComponent->OnSmoothLocationFinished.RemoveDynamic(this, &ThisClass::OnSmoothLocationFinished);
	CurrentClimbUpMontage = nullptr;
}
