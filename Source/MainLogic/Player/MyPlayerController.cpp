// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

AMyPlayerController::AMyPlayerController()
{
  
}

bool AMyPlayerController::GetIsShowShop()
{
    if (!ShopController)
    {
        return false;
    }
    return ShopController->isShowShop;
}

void AMyPlayerController::BeginPlay()
{
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("hoom ba yaya")); // 화면출력
    Initialize();
}

void AMyPlayerController::BindeDelegate()
{
    if (InventoryController && ShopController)
    {
        // 인벤토리 닫을 때 상점도 닫기
        ShopController->OnOpenShop.BindUObject(InventoryController, &UInventoryController::HandleOpenShop);

        // 상점 닫을 때 인벤토리도 닫기
        ShopController->OnCloseShop.BindUObject(InventoryController, &UInventoryController::HandleCloseShop);
    }
}

void AMyPlayerController::Initialize(UInventoryController* OldInventoryController)
{
    if (isInit)
        return;

    isInit = true;

    if (!InventoryController && InventoryControllerClass)
    {
        InventoryController = NewObject<UInventoryController>(this, InventoryControllerClass);
        InventoryController->RegisterComponent(); // 중요! Tick 등 작동하게 함
    }

    if (ShopControllerClass)
    {
        ShopController = NewObject<UShopController>(this, ShopControllerClass);
        ShopController->RegisterComponent(); // 중요! Tick 등 작동하게 함
    }

    BindeDelegate();

    if (InventoryController)
    {
        InventoryController->MyOwner = this;
        InventoryController->CopyInventory(OldInventoryController);
    }

    if (ShopController)
    {
        ShopController->MyOwner = this;
        ShopController->ShowShop();
        ShopController->CloseShop();
    }
}
