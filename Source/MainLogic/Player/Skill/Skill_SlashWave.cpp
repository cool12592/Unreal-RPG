// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill_SlashWave.h"

void USkill_SlashWave::Init_(APlayerCharacter* player, FSkillData* skillData)
{
	OwnerPlayerAnim = Cast<UMyAnimInstance>(player->GetMesh()->GetAnimInstance());
	OwnerPlayerAnim->OnSwordWaveStop.AddUObject(this, &USkill_SlashWave::PauseSlashWave);
	OwnerPlayerAnim->OnSwordWaveRepeat.AddUObject(this, &USkill_SlashWave::ShotSlashWave);
}

void USkill_SlashWave::StartSkill_Implementation()
{
	if (bIsCharging)
	{
		bIsCharging = false;
		ResumeSlashWave();
		return;
	}

	if (CheckUseSkill() == false)
		return;

	ConsumeSkillResources();

	bIsCharging = true;
	bIsFullCharge = false;

	OwnerPlayer->Oninvincibility();
	

	OwnerPlayer->PlayAnimMontage(BasicSkillMontage, 1.0f);

}



void USkill_SlashWave::PauseSlashWave()
{	
	OwnerPlayer->PlayCameraZoomIn();

	OwnerPlayerAnim->Montage_Pause(OwnerPlayer->GetCurrentMontage());

	pcCharge1 = UGameplayStatics::SpawnEmitterAttached(pCharge1, OwnerPlayer->GetMesh(), FName("Pelvis"), FVector(0.f, -130.f, 50.f), FRotator::ZeroRotator, FVector(2.F));
	pcCharge2 = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pCharge2, OwnerPlayer->GetActorLocation() - FVector::UpVector * 80.f, FRotator::ZeroRotator, FVector::OneVector * 3.f);

	pcCharge1->CustomTimeDilation = 3.5f;

	float waitTime = 1.5f;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			UpgradeWave();
		}), waitTime, false);

}

void USkill_SlashWave::ResumeSlashWave()
{
	OwnerPlayerAnim->Montage_Resume(OwnerPlayer->GetCurrentMontage());

	if(pcCharge1)
		pcCharge1->DestroyComponent();
	if (pcCharge2)
		pcCharge2->DestroyComponent();
	if (pcCharge3)
		pcCharge3->DestroyComponent();
}

void USkill_SlashWave::UpgradeWave()
{
	if (bIsCharging == false)
		return;
	pcCharge3 = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pCharge3, OwnerPlayer->GetActorLocation() - FVector::UpVector * 80.f);
	bIsFullCharge = true;
	OwnerPlayer->ResumeSequence();
}

void USkill_SlashWave::ShotSlashWave()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerPlayer;
	FRotator SpawnRotation = OwnerPlayer->GetActorRotation();
	FVector  SpawnLocation = OwnerPlayer->GetActorLocation();

	if (bIsFullCharge)
	{
		int NumProjectiles = 5;
		float AngleStep = 10.0f;

		for (int i = 0; i < NumProjectiles; i++)
		{
			float OffsetAngle = (i - (NumProjectiles - 1) / 2.0f) * AngleStep;
			FVector NewDirection = OwnerPlayer->GetActorForwardVector().RotateAngleAxis(OffsetAngle, FVector(0, 0, 1));
			FRotator NewRotation = NewDirection.Rotation();
			GetWorld()->SpawnActor<AActor>(WaveActor_BP, SpawnLocation, NewRotation, SpawnParams);
		}
	}
	else
	{
		GetWorld()->SpawnActor<AActor>(WaveActor_BP, SpawnLocation, SpawnRotation, SpawnParams);
	}
}

