// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "Misc/OutputDeviceNull.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyBase::SetState(bool flag)
{
	isAttacking = flag;
	isHited = flag;
	NowHitedState = EnemyHitedState::Normal;

}


