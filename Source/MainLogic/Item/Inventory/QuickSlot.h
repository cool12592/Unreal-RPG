// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemBase.h"
#include "QuickSlot.generated.h"


/**
 * 
 */
UCLASS()
class NOTEBOOK_API UQuickSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void LinkQuickSlot(class UInventorySlot* InvenSlot);
	void CopyQuickSlot(UQuickSlot* ThereQuickSlot);
	UFUNCTION(BlueprintCallable)
	void UseQuickSlotItem();
	void RefreshQuickSlot(ItemType ItemType, int32 Count);

	FOnItemUse OnItemUseDelegate;

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* ItemTexture;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ItemCountText;

	UPROPERTY(BlueprintReadWrite)
	int32 ItemID;
	UPROPERTY(BlueprintReadWrite)
	int32 ItemCount;
	UPROPERTY(BlueprintReadWrite)
	int32 MySlotNumber;
	UPROPERTY(BlueprintReadWrite)
	FItem_Info ItemInfo;
};
