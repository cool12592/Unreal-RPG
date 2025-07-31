// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill_DashAttack.h"
#include "Player_Skill.h"

void USkill_DashAttack::StartSkill_Implementation()
{
	if (CheckUseSkill() == false)
		return;

	ConsumeSkillResources();

	PlayerSkill->AddLinkedSkillCount();

	OwnerPlayer->PlayAnimMontage(BasicSkillMontage, 1.0f);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerPlayer;
	FRotator rotator(0.f, 0.f, 0.f);
	FVector  SpawnLocation = OwnerPlayer->GetActorLocation() + OwnerPlayer->GetActorForwardVector() * 100.f;
	GetWorld()->SpawnActor<AActor>(DashAttach_BP, SpawnLocation + FVector(0.0f, 0.f, 0.f), OwnerPlayer->GetActorRotation(), SpawnParams);

}