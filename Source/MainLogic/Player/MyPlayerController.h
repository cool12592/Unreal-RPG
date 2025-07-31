// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InventoryController.h"
#include "ShopController.h"
#include "MyPlayerController.generated.h"

UCLASS()
class NOTEBOOK_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
		AMyPlayerController();

public:
	class APlayerCharacter* Myplayer;

	bool GetIsShowShop();

	UFUNCTION(BlueprintCallable)
	void BindeDelegate();
	//void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<class UInventoryController> InventoryControllerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	TSubclassOf<class UShopController> ShopControllerClass;
	// MyPlayerController.h
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UInventoryController* InventoryController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UShopController* ShopController;

	UFUNCTION(BlueprintCallable)
	void Initialize(UInventoryController* OldInventoryController = nullptr);
private:
	bool isInit = false;
};
