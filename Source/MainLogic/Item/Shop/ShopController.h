// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShopController.generated.h"

DECLARE_DELEGATE(FOnOpenShop);
DECLARE_DELEGATE(FOnCloseShop);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class NOTEBOOK_API UShopController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShopController();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "MyUI")
	void ShowShop();
	UFUNCTION(BlueprintCallable, Category = "MyUI")
	void CloseShop();

	UFUNCTION(BlueprintCallable)
	bool TryPurchaseItem(int BuyCount, const FItem_Info& ItemInfo);

	int32 GetShopItemID(int32 Index);

	UPROPERTY()
	UUserWidget* UiShopWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isShowShop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class AMyPlayerController* MyOwner;

	FOnOpenShop OnOpenShop;
	FOnOpenShop OnCloseShop;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;	

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MyUI")
	TSubclassOf<class UUserWidget>UiShopBPClass;

private:
	UPROPERTY(EditAnywhere)
	TArray<int32> ShopItemArray;
};
