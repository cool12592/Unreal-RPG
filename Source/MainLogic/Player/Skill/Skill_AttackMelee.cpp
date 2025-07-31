// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill_AttackMelee.h"
#include "Player_Skill.h"


void USkill_AttackMelee::Init_(APlayerCharacter* player, FSkillData* skillData)
{
	auto Anim = Cast<UMyAnimInstance>(player->GetMesh()->GetAnimInstance());
	Anim->OnAttackMeleeNextCombo.AddUObject(this, &USkill_AttackMelee::ContinueNextCombo);
	Anim->OnAttackMeleeAttackCheck.AddUObject(this, &USkill_AttackMelee::ApplyAttackMeleeDamage);

	PlayerSkill->OnResetCount.AddUObject(this, &USkill_AttackMelee::ResetCount);
}

void USkill_AttackMelee::StartSkill_Implementation()
{
	IsAttackMeleeInputOn = true;

	if (AttackMeleeCount == 0)
	{
		if (CheckUseSkill() == false)
			return;

		ContinueNextCombo();
	}
}

void USkill_AttackMelee::ResetCount()
{
	AttackMeleeCount = 0;
	IsAttackMeleeInputOn = false;
}

void USkill_AttackMelee::ContinueNextCombo()
{
	if (IsAttackMeleeInputOn == false)
		return;

	if(AttackMeleeCount == 0)
		ConsumeSkillResources();

	IsAttackMeleeInputOn = false;
	AttackMeleeCount++;

	float forwardVal = 50.f;

	if (AttackMeleeCount != LastAttackMeleeCount)
	{
		OwnerPlayer->PlaySwordSound();
		forwardVal = 100.f;
	}

	OwnerPlayer->PlayAnimMontage(BasicSkillMontage, 1.5f, FName(*FString::FromInt(AttackMeleeCount)));

	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	UKismetSystemLibrary::MoveComponentTo(OwnerPlayer->GetCapsuleComponent(), OwnerPlayer->GetActorLocation() + OwnerPlayer->GetActorForwardVector() * forwardVal, OwnerPlayer->GetActorRotation(), false, false, 0.2f, true, EMoveComponentAction::Move, LatentInfo);
}

void USkill_AttackMelee::ApplyAttackMeleeDamage()
{
	if (!PlayerSkill)
		return;

	if (AttackMeleeCount == LastAttackMeleeCount)
	{
		PlayerSkill-> MyCameraShake(8.f);
	}
	else
	{
		PlayerSkill->MyCameraShake(5.f);
	}

	float attack_range = 200.f;
	float attact_radius = 200.f;
	TArray<FHitResult> HitResults;
	FVector start = OwnerPlayer->GetActorLocation();
	FVector end = OwnerPlayer->GetActorLocation() + OwnerPlayer->GetActorForwardVector() * attack_range;
	bool bResult = PlayerSkill->SweepTrace(start, end, attact_radius, HitResults);

	if (bResult == false)
		return;

	for (const FHitResult& HitResult : HitResults)
	{
		ABasicEnemy* enemyActor = Cast<ABasicEnemy>(HitResult.Actor);

		if (enemyActor == nullptr)
			continue;
		if (enemyActor->NowHitedState == EnemyHitedState::Knock)
			continue;

		UGameplayStatics::SpawnEmitterAttached(pSwordHit, enemyActor->GetMesh(), FName("Root"), FVector(0.f, 0.f, 80.f), FRotator::ZeroRotator, FVector(1.F));//Pelvis¿¡ºÙÀÓ
		FVector LaunchPower = FVector::ZeroVector;
		EnemyHitedState HitState = EnemyHitedState::RightLeft;
		float damageMultiple = 1.f;

		if (AttackMeleeCount == LastAttackMeleeCount)
		{
			OwnerPlayer->PlaySwordSound();
			LaunchPower = OwnerPlayer->GetActorForwardVector() * 300.f + OwnerPlayer->GetActorUpVector() * 500.f;
			HitState = EnemyHitedState::Knock;
			damageMultiple = 2.f;
		}
		else
		{
			if (enemyActor->isBoss == false)
			{
				FLatentActionInfo LatentInfo;
				LatentInfo.CallbackTarget = enemyActor;
				UKismetSystemLibrary::MoveComponentTo(enemyActor->GetCapsuleComponent(), enemyActor->GetActorLocation() + OwnerPlayer->GetActorForwardVector() * 110.f, enemyActor->GetActorRotation(), true, true, 0.4f, true, EMoveComponentAction::Move, LatentInfo);
			}
		}

		enemyActor->MyTakeDamage(OwnerPlayer, SkillData->Damage * damageMultiple, HitState, SkillData->StunTime, LaunchPower);
	}
}
