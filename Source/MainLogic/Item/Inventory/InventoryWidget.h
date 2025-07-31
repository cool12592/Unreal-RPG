// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemBase.h"
#include "InventoryWidget.generated.h"

DECLARE_DELEGATE(FOnInvenSlotRefreshRequested)
DECLARE_DELEGATE_RetVal(int32, FOnGoldRefreshRequested);

/**
 * 
 */
UCLASS()
class NOTEBOOK_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void BindAllInvenSlotUseItemDelegate();
	UFUNCTION(BlueprintCallable)
	void BindAllQuickSlotUseItemDelegate();
	UFUNCTION(BlueprintCallable)
	void RefreshInventory();
	UFUNCTION(BlueprintCallable)
	void RefreshGold();
	UFUNCTION(BlueprintImplementableEvent)
	void SetEquipUI();

	UFUNCTION(BlueprintCallable)
	void ChangeQuickSlots(TArray<class UQuickSlot*> NewQuickSlotArray);

	FOnInvenSlotRefreshRequested OnInvenSlotRefreshRequested;
	FOnGoldRefreshRequested OnGoldRefreshRequested;
	FOnItemUse OnItemUseDelegate;


	UPROPERTY(BlueprintReadWrite)
	TArray<class UInventorySlot*> InvenSlotArray;
	UPROPERTY(BlueprintReadWrite)
	TArray<class UQuickSlot*> QuickSlotArray;
	UPROPERTY(BlueprintReadWrite)
	class AMyPlayerController* MyOwner;
protected:
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* GoldText;

private:
	void RefreshInvenSlot();
};
