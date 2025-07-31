// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HorseCharacter.generated.h"

UCLASS()
class NOTEBOOK_API AHorseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHorseCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int isRight = 0;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool isCanMove = true;

private:	

	UFUNCTION()
	void MoveForWard(float value);
	UFUNCTION()
	void MoveRight(float value);
	UFUNCTION()
	void StartJump();

	void CalculateBodyLeanDirection(float DeltaTime);

	FVector OldForward;

	UPROPERTY()
	float DirectionCount = 0.f;
};
