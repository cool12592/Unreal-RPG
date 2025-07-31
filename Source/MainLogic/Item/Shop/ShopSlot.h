// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemBase.h"
#include "ShopSlot.generated.h"


DECLARE_DELEGATE_RetVal_OneParam(bool, FTrySelectSlotDelegate, UShopSlot*);

/**
 * 
 */
UCLASS()
class NOTEBOOK_API UShopSlot : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void InitSlot(int32 itemID, class UDataTable* itemDataTable);
    UFUNCTION(BlueprintCallable)
    void CancelClick();

    UFUNCTION(BlueprintCallable)
    void HandleSlotClicked();

    FTrySelectSlotDelegate TrySelectSlotDelegate;

    UPROPERTY(BlueprintReadWrite)
    FItem_Info ItemInfo;

protected:
    // À§Á¬ ¹ÙÀÎµù
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UImage* ItemImage;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UImage* SelectImage;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* ItemCost;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* SlotButton;


    UPROPERTY(BlueprintReadWrite)
    int32 ItemID;

    bool isInit = false;
};
