// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbHandlers/ClimbHandlerBase.h"

void UClimbHandlerBase::Init()
{
    ReceiveInit();
}

void UClimbHandlerBase::EndClimb()
{
    ReceiveEndClimb();
}

bool UClimbHandlerBase::StartClimb(FHitResult HitResult)
{
    return ReceiveStartClimb(HitResult);
}

void UClimbHandlerBase::HandleMovement(FVector2D MoveDirection)
{
    ReceiveHandleMovement(MoveDirection);
}
