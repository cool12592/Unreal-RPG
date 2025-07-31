// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill_AirCombo.h"
#include "Player_Skill.h"


void USkill_AirCombo::Init_(APlayerCharacter* player, FSkillData* skillData)
{
	PlayerSkill->OnResetCount.AddUObject(this, &USkill_AirCombo::ResetCount);

	auto Anim = Cast<UMyAnimInstance>(player->GetMesh()->GetAnimInstance());
	Anim->OnAirComboAttackCheck.AddUObject(this, &USkill_AirCombo::ApplyAirComboDamage);
	Anim->OnAirComboNextCombo.AddUObject(this, &USkill_AirCombo::ContinueNextCombo);
	Anim->OnSwordDance.AddUObject(this, &USkill_AirCombo::ApplySlashStormDamage);

	AirComboPosArray.Add(FVector(0.f, 0.f, -80.f));
	AirComboPosArray.Add(FVector(320.000305f, 236.602325f, 50.f));
	AirComboPosArray.Add(FVector(149.084198f, -227.087494f, 20.115582f));
	AirComboPosArray.Add(FVector(15.924797f, 264.118744f, 33.115582f));

	AirComboRotationArray.Add(FRotator(0.f, -90.f, 0.f));
	AirComboRotationArray.Add(FRotator(10.000835f, -220.000122f, 0.00013f));
	AirComboRotationArray.Add(FRotator(0.f, 0.f, 0.f));
	AirComboRotationArray.Add(FRotator(0.f, 200.000595f, 0.f));
}

void USkill_AirCombo::StartSkill_Implementation()
{
	if (AirComboCount == 0)
	{
		if (CheckUseSkill() == false)
			return;

		ConsumeSkillResources();

		bIsMaxLinkedSkillCount = false;
		CurrentSkillMontage = BasicSkillMontage;
		if (PlayerSkill->IsMaxLinkedSkillCount())
		{
			CurrentSkillMontage = UpgradeSkillMontage;
			bIsMaxLinkedSkillCount = true;
		}

		if (OwnerPlayer->GetCharacterMovement()->IsMovingOnGround() == false) //이미공중이면 첫번째동작은 노필요
		{
			AirComboCount++; //첫번째동작거름
			HoldCharacter(OwnerPlayer);
		}

		AirComboCount++;

		OwnerPlayer->PlayAnimMontage(CurrentSkillMontage, 1.f, FName(*FString::FromInt(AirComboCount)));

		bAirComboMeleeInputOn = false;
	}
	else //공격중
	{
		bAirComboMeleeInputOn = true;
	}

}

void USkill_AirCombo::ResetCount()
{
	bAirComboMeleeInputOn = false;
	LastHitEnemy = nullptr;

	AirComboCount = 0;
	OwnerPlayer->GetCharacterMovement()->GravityScale = 1.f;
}

void USkill_AirCombo::ContinueNextCombo()
{
	if (bAirComboMeleeInputOn)
	{
		bAirComboMeleeInputOn = false;

		int Index = (AirComboCount - 1) % 4;
		OwnerPlayer->GetMesh()->SetRelativeLocationAndRotation(AirComboPosArray[Index], AirComboRotationArray[Index]);

		AirComboCount++;
		OwnerPlayer->PlayAnimMontage(CurrentSkillMontage, 1.f, FName(*FString::FromInt(AirComboCount)));

		if (AirComboCount == 2)
			HoldCharacter(OwnerPlayer);

		PlayerSkill->MyCameraShake(7.f);
	}
	else
	{
		OwnerPlayer->GetMesh()->SetRelativeLocationAndRotation(AirComboPosArray[0], AirComboRotationArray[0]);
	}
}

void USkill_AirCombo::ApplyAirComboDamage()
{
	if (AirComboCount == 1)
		OwnerPlayer->LaunchCharacter(FVector(0, 0, LaunchPower_AirCombo), false, false);

	TArray<FHitResult> HitResults;
	float range = 100.f;
	float radius = 200.f;
	FVector start = OwnerPlayer->GetActorLocation();
	FVector end = OwnerPlayer->GetActorLocation() + OwnerPlayer->GetActorForwardVector() * range;
	bool bResult = PlayerSkill->SweepTrace(start, end, radius, HitResults);

	LastHitEnemy = nullptr;

	if (bResult)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			ABasicEnemy* enemyActor = Cast<ABasicEnemy>(HitResult.Actor);
			if (enemyActor)
			{
				LastHitEnemy = enemyActor;
				if (enemyActor->NowHitedState == EnemyHitedState::Knock) continue;

				if (AirComboCount != LastAirComboCount2)
				{
					enemyActor->MyTakeDamage(OwnerPlayer, SkillData->Damage, EnemyHitedState::RightLeft, 2.f);
					enemyActor->StartResetGravityTimer(); //내가 때리다 말수있으므로 착륙 타이머시킴

					if (AirComboCount == 1)
						enemyActor->LaunchCharacter(FVector(0, 0, LaunchPower_AirCombo), false, false);
					else if (AirComboCount == 2)
						HoldCharacter(enemyActor);
				}
				else
				{
					enemyActor->GetCharacterMovement()->GravityScale = 1.f;
					enemyActor->MyTakeDamage(OwnerPlayer, SkillData->Damage, EnemyHitedState::Knock, 3.f, FVector(0, 0, -300.f));
					enemyActor->ClearGravityTimer();//타이머중인건꺼야지혼선방지
				}
			}
		}
	}

	CheckFinishAttack();
	PlayerSkill->MyCameraShake(7.f);
}

void USkill_AirCombo::CheckFinishAttack()
{
	if (bIsMaxLinkedSkillCount == false)
		return;
	if (AirComboCount != LastAirComboCount2)
		return;

	if (LastHitEnemy != nullptr)
	{
		ReadySlashStorm();
	}
	else
	{
		PlayerSkill->ResetCount();
	}
}

void USkill_AirCombo::ReadySlashStorm()
{
	
	OwnerPlayer->PlayTimeDilationEffect();

	OwnerPlayer->MyAnim->Montage_SetPlayRate(OwnerPlayer->GetCurrentMontage(), 0.2f);
	
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pTeleportCharge, OwnerPlayer->GetActorLocation() + FVector(0.f, -70.f, 0.f))->CustomTimeDilation = 3.f;

	float WaitTime1 = 0.7f;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			OwnerPlayer->CustomTimeDilation = 0.1f;
			SlowTargets();

			float WaitTime2 = 1.6f;

			GetWorld()->GetTimerManager().SetTimer(WaitHandle2, FTimerDelegate::CreateLambda([&]()
				{
					HoldTargets();
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pTeleportEnter, OwnerPlayer->GetActorLocation())->CustomTimeDilation = 0.5f;
					
					OwnerPlayer->SetActorLocation(LastHitEnemy->GetActorLocation() - OwnerPlayer->GetActorForwardVector() * 300.f);
					HoldCharacter(OwnerPlayer);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pTeleportExit, OwnerPlayer->GetActorLocation())->CustomTimeDilation = 0.5f;;

					StartSlashStorm();
					OwnerPlayer->CustomTimeDilation = 1.f;

				}), WaitTime2, false);

		}), WaitTime1, false);
}

void USkill_AirCombo::StartSlashStorm()
{
	OwnerPlayer->PlayAnimMontage(SlashStormMontage, 1.0f);
}

void USkill_AirCombo::ApplySlashStormDamage()
{
	TArray<FHitResult> HitResults;
	float range = 400.f;
	float radius = 400.f;
	FVector start = OwnerPlayer->GetActorLocation();
	FVector end = OwnerPlayer->GetActorLocation() + OwnerPlayer->GetActorForwardVector() * range;
	bool bResult = PlayerSkill->SweepTrace(start, end, radius, HitResults);

	if (bResult)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			ABasicEnemy* enemyActor = Cast<ABasicEnemy>(HitResult.Actor);
			if (enemyActor && !enemyActor->GetCharacterMovement()->IsMovingOnGround())
			{
				enemyActor->CustomTimeDilation = 1.f;
				enemyActor->MyTakeDamage(OwnerPlayer, SkillData->Damage, EnemyHitedState::Knock, 1.f);
			}
		}
	}
	PlayerSkill->MyCameraShake(20.f);
}

void USkill_AirCombo::HoldTargets()
{

	TArray<FHitResult> HitResults;
	float range = 800.f;
	float radius = 800.f;
	FVector start = OwnerPlayer->GetActorLocation();
	FVector end = OwnerPlayer->GetActorLocation() + OwnerPlayer->GetActorForwardVector() * range;
	bool bResult = PlayerSkill->SweepTrace(start, end, radius, HitResults);

	if (bResult)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			ABasicEnemy* enemyActor = Cast<ABasicEnemy>(HitResult.Actor);
			if (enemyActor && !enemyActor->GetCharacterMovement()->IsMovingOnGround())
			{
				HoldCharacter(enemyActor);
				enemyActor->StartResetGravityTimer(2.5f);
			}
		}
	}
}


void USkill_AirCombo::SlowTargets()
{
	TArray<FHitResult> HitResults;
	float range = 500.f;
	float radius = 500.f;
	FVector start = OwnerPlayer->GetActorLocation();
	FVector end = OwnerPlayer->GetActorLocation() + OwnerPlayer->GetActorForwardVector() * range;
	bool bResult = PlayerSkill->SweepTrace(start, end, radius, HitResults);

	if (bResult)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			ABasicEnemy* enemyActor = Cast<ABasicEnemy>(HitResult.Actor);
			if (enemyActor && !enemyActor->GetCharacterMovement()->IsMovingOnGround())
			{
				enemyActor->CustomTimeDilation = 0.07f;
				enemyActor->StartResetGravityTimer(3.f);

			}
		}
	}
}

void USkill_AirCombo::HoldCharacter(ACharacter* character)
{
	character->GetCharacterMovement()->GravityScale = 0.f;
	character->GetCharacterMovement()->StopMovementImmediately();
}