// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbHandlers/ClimbHandlerBase.h"

void UClimbHandlerBase::Init()
{
    ReceiveInit();
}

void UClimbHandlerBase::StartClimb(FHitResult HitResult)
{
    ReceiveStartClimb(HitResult);
}

FVector2D UClimbHandlerBase::HandleMovement(FVector2D MoveDirection)
{
    return ReceiveHandleMovement(MoveDirection);
}
