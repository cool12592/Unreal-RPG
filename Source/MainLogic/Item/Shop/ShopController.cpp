// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopController.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "PlayerCharacter.h"
#include "MyPlayerController.h"
#include "ShopWidget.h"

// Sets default values for this component's properties
UShopController::UShopController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UShopController::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UShopController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UShopController::ShowShop()
{
	if (isShowShop)
		return;

	if (!UiShopWidget)
	{
		if (!UiShopBPClass)
			return;

		if (UShopWidget* ShopWidget = CreateWidget<UShopWidget>(GetWorld(), UiShopBPClass))
		{
			ShopWidget->OnGetShopItemIDDelegate.BindUObject(this, &UShopController::GetShopItemID);
			ShopWidget->OnTryPurchaseItemDelegate.BindUObject(this, &UShopController::TryPurchaseItem);

			UiShopWidget = ShopWidget;
		}
	}


	if (UiShopWidget && UiShopWidget->IsInViewport() == false)
		UiShopWidget->AddToViewport();

	if (MyOwner)
	{
		MyOwner->bShowMouseCursor = true;
		MyOwner->SetInputMode(FInputModeGameAndUI());
	}

	isShowShop = true;

	OnOpenShop.ExecuteIfBound();
}

void UShopController::CloseShop()
{
	if (MyOwner)
	{
		MyOwner->bShowMouseCursor = false;
		MyOwner->SetInputMode(FInputModeGameOnly());
	}

	if (UiShopWidget)
	{
		UiShopWidget->RemoveFromParent();
		UiShopWidget->Destruct();
		UiShopWidget = nullptr;
	}

	isShowShop = false;
	OnCloseShop.ExecuteIfBound();
}

bool UShopController::TryPurchaseItem(int BuyCount, const FItem_Info& ItemInfo)
{
	if (!MyOwner)
		return false;

	bool purchaseResult = false;
	int cost = ItemInfo.ItemGold * BuyCount;
	if (cost < MyOwner->GetMyGold())
	{
		if (MyOwner->InventoryController)
		{
			MyOwner->InventoryController->AddToInventory(ItemInfo.ItemType, BuyCount);
			MyOwner->InventoryController->ChangeMyGold(-cost);
			purchaseResult = true;
		}
	}

	return purchaseResult;
}

int32 UShopController::GetShopItemID(int32 Index)
{
	return ShopItemArray.IsValidIndex(Index) ? ShopItemArray[Index] : 0;
}
