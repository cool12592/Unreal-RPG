// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill_GreatSword.h"
#include "Player_Skill.h"

void USkill_GreatSword::Init_(APlayerCharacter* player, FSkillData* skillData)
{
	auto Anim = Cast<UMyAnimInstance>(player->GetMesh()->GetAnimInstance());
	Anim->OnMagicSwordAttackCheck.AddUObject(this, &USkill_GreatSword::ApplyGreatSwordDamage);
}

void USkill_GreatSword::StartSkill_Implementation()
{
	if (CheckUseSkill() == false)
		return;

	ConsumeSkillResources();

	OwnerPlayer->Oninvincibility();
	OwnerPlayer->PlayAnimMontage(BasicSkillMontage, 1.0f);
	ExecuteWideStun();

	OwnerPlayer->PlayGreatSwordCameraEffect();
}

void USkill_GreatSword::ExecuteWideStun()
{
	TArray<FHitResult> HitResults;
	float radius = 500.f;
	FVector start = OwnerPlayer->GetActorLocation() + OwnerPlayer->GetActorForwardVector() * 400.f;
	FVector end = OwnerPlayer->GetActorLocation() + OwnerPlayer->GetActorForwardVector() * radius;
	bool bResult = PlayerSkill->SweepTrace(start, end, radius, HitResults);

	float StunDamage = 10.f;
	float StunTime = 5.f;

	if (bResult == false)
		return;

	for (const FHitResult& HitResult : HitResults)
	{
		ABasicEnemy* enemyActor = Cast<ABasicEnemy>(HitResult.Actor);
		if (enemyActor)
		{
			enemyActor->MyTakeDamage(OwnerPlayer, StunDamage, EnemyHitedState::RightLeft, StunTime);
		}
	}
}
void USkill_GreatSword::ApplyGreatSwordDamage()
{
	TArray<FHitResult> HitResults;
	float radius = 500.f;
	FVector start = OwnerPlayer->GetActorLocation() + OwnerPlayer->GetActorForwardVector() * 400.f;
	FVector end = OwnerPlayer->GetActorLocation() + OwnerPlayer->GetActorForwardVector() * radius;
	bool bResult = PlayerSkill->SweepTrace(start, end, radius, HitResults);

	if (bResult)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			ABasicEnemy* enemyActor = Cast<ABasicEnemy>(HitResult.Actor);
			if (enemyActor)
			{
				enemyActor->MyTakeDamage(OwnerPlayer, SkillData->Damage, EnemyHitedState::Knock, SkillData->StunTime, OwnerPlayer->GetActorUpVector() * 500.f);
			}
		}
	}

	PlayerSkill->MyCameraShake(20.f);

}