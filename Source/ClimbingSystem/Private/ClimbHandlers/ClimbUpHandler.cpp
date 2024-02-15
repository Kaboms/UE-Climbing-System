// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbHandlers/ClimbUpHandler.h"
#include "Components/ClimbingComponent.h"
#include "GameFramework/Character.h"

bool UClimbUpHandler::FindAndStartClimbUp(float EdgeZPos)
{
	float EdgeToCharacterCoof = ClimbingComponentBase->GetEdgeToCharacterRatio(EdgeZPos);

	for (const auto& ClimbUpMontage : ClimbUpMontages)
	{
		if (FMath::IsNearlyEqual(EdgeToCharacterCoof, ClimbUpMontage.Key, ClimbUpErrorTolerance))
		{
			ClimbingComponentBase->GetOwnerCharacter()->SetActorLocation(ClimbingComponentBase->GetPositionToEdgeWithOffset(EdgeZPos, ClimbUpMontage.Key));

			ClimbUp(ClimbUpMontage.Value);

			return true;
		}
	}

	return false;
}
