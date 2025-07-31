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
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("hoom ba yaya")); // ȭ�����
    Initialize();
}

void AMyPlayerController::BindeDelegate()
{
    if (InventoryController && ShopController)
    {
        // �κ��丮 ���� �� ������ �ݱ�
        ShopController->OnOpenShop.BindUObject(InventoryController, &UInventoryController::HandleOpenShop);

        // ���� ���� �� �κ��丮�� �ݱ�
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
        InventoryController->RegisterComponent(); // �߿�! Tick �� �۵��ϰ� ��
    }

    if (ShopControllerClass)
    {
        ShopController = NewObject<UShopController>(this, ShopControllerClass);
        ShopController->RegisterComponent(); // �߿�! Tick �� �۵��ϰ� ��
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
