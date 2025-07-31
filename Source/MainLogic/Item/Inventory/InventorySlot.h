// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemBase.h"
#include "InventorySlot.generated.h"


/**
 * 
 */
UCLASS()
class NOTEBOOK_API UInventorySlot : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void RefreshSlot(int32 itemID, int32 itemCount, class UDataTable* itemDataTable);
	UFUNCTION(BlueprintCallable)
	void UseSlotItem();
	void SettingEmptySlot();

	FOnItemUse OnItemUseDelegate;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* ItemTexture;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ItemCountText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCanvasPanel* WeaponUI_Canvas;
	UPROPERTY(BlueprintReadWrite)
	int32 ItemID;
	UPROPERTY(BlueprintReadWrite)
	int32 ItemCount;
	UPROPERTY(BlueprintReadWrite)
	FItem_Info ItemInfo;
};
