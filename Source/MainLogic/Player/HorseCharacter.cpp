// Fill out your copyright notice in the Description page of Project Settings.


#include "HorseCharacter.h"

// Sets default values
AHorseCharacter::AHorseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AHorseCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AHorseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CalculateBodyLeanDirection(DeltaTime);
}

// Called to bind functionality to input
void AHorseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForWard", this, &AHorseCharacter::MoveForWard);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHorseCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AHorseCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AHorseCharacter::AddControllerPitchInput);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AHorseCharacter::StartJump);
}

void AHorseCharacter::MoveForWard(float value)
{
	if (!Controller) return;
	if (value == 0) return;
	if (isCanMove == false) return;

	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, value);
}

void AHorseCharacter::MoveRight(float value)
{
	if (!Controller) return;
	if (value == 0) return;
	if (isCanMove == false) return;

	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, value);
}

void AHorseCharacter::StartJump()
{
	if(GetVelocity().Size()>1200)
		bPressedJump = true;
}

void AHorseCharacter::CalculateBodyLeanDirection(float DeltaTime)
{
	if (OldForward != GetActorForwardVector())
	{
		auto dotRes = FVector::DotProduct(GetActorRightVector(), GetActorForwardVector() - OldForward);
		if (dotRes > 0)//¿ìÈ¸Àü
		{
			if (DirectionCount < 0.f)
				DirectionCount = 0.f;

			DirectionCount += DeltaTime;
			if (DirectionCount >= 0.2f)
				isRight = 1;
		}
		else if (dotRes < 0)
		{
			if (DirectionCount > 0.f)
				DirectionCount = 0.f;

			DirectionCount -= DeltaTime;
			if (DirectionCount <= -0.2f)
				isRight = -1;
		}
	}
	else
	{
		DirectionCount = 0.f;
		isRight = 0;
	}
	OldForward = GetActorForwardVector();
}
