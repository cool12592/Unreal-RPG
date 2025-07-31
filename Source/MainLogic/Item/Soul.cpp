// Fill out your copyright notice in the Description page of Project Settings.


#include "Soul.h"
#include "PlayerCharacter.h"

// Sets default values
ASoul::ASoul()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASoul::BeginPlay()
{
	Super::BeginPlay();
}

void ASoul::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    if (!isReady)
        return;

    APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor);
    player->AddToGold(Gold);
    isReady = false;
    Destroy();
}

// Called every frame
void ASoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (!isReady) 
        return;

    if (!IsValid(Target)) 
        return;

    // ���� ��ġ�� Ÿ�� ��ġ ��������
    FVector MyLocation = GetActorLocation();
    FVector TargetLocation = Target->GetActorLocation();

    // Z�� �Ÿ� �� ���
    float ZDiff = FMath::Abs(TargetLocation.Z - MyLocation.Z);

    if (ZDiff <= MaxZDiff)
    {
        float Alpha = DeltaTime * MoveSpeed;

        FVector NewLocation = FMath::Lerp(MyLocation, TargetLocation, Alpha);

        SetActorLocation(NewLocation);
    }

}

// Called every frame
void ASoul::ReadySoul(int Gold_, AActor* target_)
{
	Gold = Gold_; 
	Target = target_;  

	FTimerHandle WaitHandle;
	float WaitTime =0.5f; //�ð��� �����ϰ�
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			isReady = true;
		}), WaitTime, false); //�ݺ��� ���⼭ �߰� ������ ������ ��������

}