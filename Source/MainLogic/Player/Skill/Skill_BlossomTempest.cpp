// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill_BlossomTempest.h"
#include "Player_Skill.h"

void USkill_BlossomTempest::Init_(APlayerCharacter* player, FSkillData* skillData)
{
	auto Anim = Cast<UMyAnimInstance>(player->GetMesh()->GetAnimInstance());

	Anim->OnAttackHitCheck.AddUObject(this, &USkill_BlossomTempest::ApplyBlossomDamage);
	Anim->OnUltimate1NextCombo.AddUObject(this, &USkill_BlossomTempest::ContinueNextCombo);
}

void USkill_BlossomTempest::StartSkill_Implementation()
{
	if (CheckUseSkill() == false)
		return;

	ConsumeSkillResources();

	OwnerPlayer->Oninvincibility();
	ComboRepeat = 0;
	ComboCount = 1;

	bIsFirst = true;

	FString PlaySection = "attack" + FString::FromInt(ComboCount);

	//몽타주 포인터, 재생속도(1이 기본), 섹션이름
	float speed = 1.0f;
	OwnerPlayer->PlayAnimMontage(BasicSkillMontage, speed, FName(*PlaySection)); //string을 name으로 형변환 (원래 이함수가 name을 받아서) 참고로 string으로 먼저 쓴이유는 문자조작되는게 string이라서

	OwnerPlayer->CustomTimeDilation = 0.5f;

	TArray<FHitResult> HitResults;
	SlowTargets(HitResults);
}

void USkill_BlossomTempest::ContinueNextCombo()
{
	ComboCount++;

	if (ComboCount == 4)
	{
		if (ComboRepeat < 6)
		{
			ComboCount = 2;
			ComboRepeat++;
		}
		else
		{
			ComboRepeat = 0;
		}
	}


	if (ComboCount == 2 && ComboRepeat == 0)
	{
		OwnerPlayer->CreateWall();
	}

	FString PlaySection = "attack" + FString::FromInt(ComboCount);

	float speed = 1.5f;
	if (ComboCount == 4)
		speed = 1.f;
	OwnerPlayer->PlayAnimMontage(BasicSkillMontage, speed, FName(*PlaySection)); //string을 name으로 형변환 (원래 이함수가 name을 받아서) 참고로 string으로 먼저 쓴이유는 문자조작되는게 string이라서

}

void USkill_BlossomTempest::ApplyBlossomDamage()
{
	TArray<FHitResult> HitResults;
	SlowTargets(HitResults);

	bool bIsExistEnemy = false;

	for (const FHitResult& HitResult : HitResults)
	{
		ABasicEnemy* enemyActor = Cast<ABasicEnemy>(HitResult.Actor);
		if (enemyActor)
		{
			bIsExistEnemy = true;


			if (ComboCount == 4)
			{
				enemyActor->GetCharacterMovement()->GravityScale = 1;
				enemyActor->MyTakeDamage(OwnerPlayer, SkillData->Damage, EnemyHitedState::Knock, 5.f, FVector(0, 0, -1000));
			}
			else
			{
				enemyActor->MyTakeDamage(OwnerPlayer, SkillData->Damage, EnemyHitedState::RightLeft, SkillData->StunTime);
			}

			if (ComboRepeat == 0)
			{
				//띄우기
				if (ComboCount == 1)
				{
					enemyActor->GetCharacterMovement()->StopMovementImmediately();
					enemyActor->LaunchCharacter(LaunchPower, true, true);

					if (bIsFirst)
					{
						OwnerPlayer->LaunchCharacter(LaunchPower, true, true);
						FTimerHandle TH;
						OwnerPlayer->GetWorldTimerManager().SetTimer(TH, this, &USkill_BlossomTempest::ContinueNextCombo, 1.2f, false);
						bIsFirst = false;

						OwnerPlayer->CustomTimeDilation = 1.f;
						enemyActor->CustomTimeDilation = 1.f;
					}
				}
				//공중 고정
				if (ComboCount == 2)
				{
					if (OwnerPlayer->GetCharacterMovement()->GravityScale != 0.f)
					{
						HoldCharacter(OwnerPlayer);
					}
					HoldCharacter(enemyActor);
				}
			} 
		}
	}
	
	if (!bIsExistEnemy) //공격대상없음
	{
		PlayerSkill->ResetCount();
		OwnerPlayer->CustomTimeDilation = 1.f;
		return;
	}

	if (ComboCount != 1)
	{
		PlayerSkill->MyCameraShake(20.f);
	}
}

void USkill_BlossomTempest::HoldCharacter(ACharacter* character)
{
	character->GetCharacterMovement()->GravityScale = 0.f;
	character->GetCharacterMovement()->StopMovementImmediately();
}

void USkill_BlossomTempest::SlowTargets(TArray<FHitResult>& OutHitResults)
{
	float radius = 500.f;
	FVector start = OwnerPlayer->GetActorLocation();
	FVector end = OwnerPlayer->GetActorLocation() + OwnerPlayer->GetActorForwardVector() * 500.f;
	bool bResult = PlayerSkill->SweepTrace(start, end, radius, OutHitResults);

	if (bResult)
	{
		for (const FHitResult& HitResult : OutHitResults)
		{
			ABasicEnemy* enemyActor = Cast<ABasicEnemy>(HitResult.Actor);
			if (enemyActor)
			{
				enemyActor->CustomTimeDilation = 0.5f;
			}
		}
	}
}
