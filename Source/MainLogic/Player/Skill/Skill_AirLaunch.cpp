// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill_AirLaunch.h"
#include "Player_Skill.h"


void USkill_AirLaunch::Init_(APlayerCharacter* player, FSkillData* skillData)
{
	auto Anim = Cast<UMyAnimInstance>(player->GetMesh()->GetAnimInstance());

	Anim->OnUpperWindCheck.AddUObject(this, &USkill_AirLaunch::ApplyAirLaunchDamage);

	PlayerSkill->OnResetCount.AddUObject(this, &USkill_AirLaunch::ResetCount);
}

void USkill_AirLaunch::StartSkill_Implementation()
{
	if (CheckUseSkill() == false)
		return;

	ConsumeSkillResources();

	PlayerSkill->AddLinkedSkillCount();

	OwnerPlayer->SetActorEnableCollision(false);
	OwnerPlayer->GetCharacterMovement()->StopMovementImmediately();
	OwnerPlayer->LaunchCharacter(FVector(0, 0, LaunchPower), false, false);

	OwnerPlayer->PlayAnimMontage(BasicSkillMontage, 1.0f);

	FTimerHandle WaitHandle;
	float WaitTime = 1.f;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			OwnerPlayer->SetActorEnableCollision(true);
		}), WaitTime, false);
}

void USkill_AirLaunch::ResetCount()
{
	AirLaunchCount = 0.f;
}

void USkill_AirLaunch::ApplyAirLaunchDamage()
{
	TArray<FHitResult> HitResults;
	float radius = 250.f;
	FVector start = OwnerPlayer->GetActorLocation();
	FVector end = OwnerPlayer->GetActorLocation() + OwnerPlayer->GetActorForwardVector() * radius;
	bool bResult = PlayerSkill->SweepTrace(start, end, radius, HitResults);

	if (bResult)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			ABasicEnemy* enemyActor = Cast<ABasicEnemy>(HitResult.Actor);
			if (enemyActor)
			{
				if (AirLaunchCount == 0)
				{
					enemyActor->GetCharacterMovement()->StopMovementImmediately();
					enemyActor->MyTakeDamage(OwnerPlayer, SkillData->Damage, EnemyHitedState::RightLeft, 3.f, FVector(0.f, 0.f, 1420.f));
				}
				else
					enemyActor->MyTakeDamage(OwnerPlayer, SkillData->Damage, EnemyHitedState::RightLeft, 3.f);
			}
		}
	}

	PlayerSkill->MyCameraShake(10.f);

	AirLaunchCount++;

}