// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Soul.generated.h"

UCLASS()
class NOTEBOOK_API ASoul : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoul();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void ReadySoul(int Gold_, AActor* target_);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Gold = 50;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isReady = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Target;

	static constexpr float MoveSpeed = 100.f;
	static constexpr float MaxZDiff = 1000.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
