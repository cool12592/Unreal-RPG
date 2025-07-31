// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryController.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "PlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "MyPlayerController.h"
#include "InventoryWidget.h"
#include "InventorySlot.h"
#include "QuickSlot.h"
#include "Engine.h"
// Sets default values for this component's properties
UInventoryController::UInventoryController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
	isShowInventory = false;
	AddToInventory(ItemType::BasicWeapon, 1);
	AddToInventory(ItemType::HP_Potion, 3);
}

void UInventoryController::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryController::RefreshInventorySlots()
{
	if (!InventoryWidget)
		return;

	TArray<ItemType> KeyArray;
	InvenItemMap.GetKeys(KeyArray);

	const int32 SlotNum = InventoryWidget->InvenSlotArray.Num();
	const int32 MapKeyNum = KeyArray.Num();

	for (int32 index = 0; index < SlotNum; index++)
	{
		if (index < MapKeyNum)
		{
			ItemType type = KeyArray[index];
			int32 ItemID = GetItemIDFromType(type);
			int32 Count = InvenItemMap[type];

			InventoryWidget->InvenSlotArray[index]->RefreshSlot(ItemID, Count, ItemDataTable);
			for (int i = 0; i < InventoryWidget->QuickSlotArray.Num(); i++)
			{
				InventoryWidget->QuickSlotArray[i]->RefreshQuickSlot(type, Count);
			}
		}
		else
		{
			// 슬롯이 남은 경우 초기화
			InventoryWidget->InvenSlotArray[index]->RefreshSlot(0, 0, nullptr);
		}
	}
	
}

int32 UInventoryController::GetItemIDFromType(ItemType ItemType)
{
	return static_cast<int32>(ItemType) + 1;
}

// Called every frame
void UInventoryController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInventoryController::CopyInventory(const UInventoryController* other)
{
	if (other==nullptr) 
		return;

	InvenItemMap.Empty();
	MyGold = other->MyGold;
	InvenItemMap = other->InvenItemMap;
	WearingWeapon = other->WearingWeapon;
	InventoryWidget = other->InventoryWidget;
	InitInventoryWidget();
	InventoryWidget->BindAllInvenSlotUseItemDelegate();
}

void UInventoryController::InitInventoryWidget()
{
	InventoryWidget->OnInvenSlotRefreshRequested.BindUObject(this, &UInventoryController::RefreshInventorySlots);
	InventoryWidget->OnGoldRefreshRequested.BindUObject(this, &UInventoryController::GetGold);
	InventoryWidget->OnItemUseDelegate.BindUObject(this, &UInventoryController::UseItem);
	InventoryWidget->MyOwner = MyOwner;
}


void UInventoryController::ShowInventory()
{
	if (isShowInventory)
		return;

	if (!InventoryWidget)
	{
		InventoryWidget = CreateWidget<UInventoryWidget>(GetWorld(), UiInventoryBPClass);
		InitInventoryWidget();
	}


	if (InventoryWidget->IsInViewport() == false)
		InventoryWidget->AddToViewport();

	InventoryWidget->SetVisibility(ESlateVisibility::Visible);

	if (MyOwner)
	{
		MyOwner->bShowMouseCursor = true;
		MyOwner->SetInputMode(FInputModeGameAndUI());
	}
	
	isShowInventory = true;
}

void UInventoryController::CloseInventory()
{
	if (MyOwner->GetIsShowShop()) 
		return;

	if (InventoryWidget)
		InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
	
	if (MyOwner)
	{
		MyOwner->bShowMouseCursor = false;
		MyOwner->SetInputMode(FInputModeGameOnly());
	}
	
	isShowInventory = false;
}

void UInventoryController::AddToInventory(ItemType itemType, int count)
{
	if (InvenItemMap.Contains(itemType))
		InvenItemMap[itemType] += count;
	else
		InvenItemMap.Add(itemType, count);
}

void UInventoryController::HandleOpenShop()
{
	if (InventoryWidget && InventoryWidget->IsInViewport())
	{
		isShowInventory = false;
		InventoryWidget->RemoveFromViewport();
	}

	ShowInventory();
}

void UInventoryController::HandleCloseShop()
{
	CloseInventory();
}


void UInventoryController::ChangeMyGold(int AddGold)
{
	MyGold += AddGold;
}

void UInventoryController::UseItem_Implementation(ItemType itemType)
{
	UseItem_Multicast(itemType);	
}

void UInventoryController::UseItem_Multicast_Implementation(ItemType itemType)
{
	if (!MyOwner || !InventoryWidget)
		return;

	if (!InvenItemMap.Find(itemType))
		return;

	auto* player = Cast<APlayerCharacter>(MyOwner->GetCharacter());
	if (!player  || !player->CheckCanUseItem())
		return;


	switch (itemType)
	{
	case ItemType::HP_Potion:
	{
		InvenItemMap[itemType]--;
		player->HP_HEAL(20.F);
		break;
	}
	case ItemType::Stamina_Potion:
	{
		InvenItemMap[itemType]--;
		player->STAMINA_HEAL(20.F);
		break;
	}
	case ItemType::BasicWeapon:
	{
		WearingWeapon = ItemType::BasicWeapon;
		FOutputDeviceNull pAR;
		player->CallFunctionByNameWithArguments(TEXT("WeaponChange"), pAR, nullptr, true);
		InventoryWidget->SetEquipUI();
		break;
	}
	case ItemType::SpecialWeapon:
	{
		WearingWeapon = ItemType::SpecialWeapon;
		FOutputDeviceNull pAR;
		player->CallFunctionByNameWithArguments(TEXT("WeaponChange"), pAR, nullptr, true);
		InventoryWidget->SetEquipUI();
		break;
	}
	default:
		break;
	}

	for (int i = 0; i < InventoryWidget->QuickSlotArray.Num(); i++)
	{
		InventoryWidget->QuickSlotArray[i]->RefreshQuickSlot(itemType, InvenItemMap[itemType]);
	}

	if (InvenItemMap[itemType] <= 0)
		InvenItemMap.Remove(itemType);

	InventoryWidget->RefreshInventory();
}
