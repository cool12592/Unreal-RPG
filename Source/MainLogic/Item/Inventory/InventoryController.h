// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemBase.h"
#include "InventoryController.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class NOTEBOOK_API UInventoryController : public UActorComponent
{
	GENERATED_BODY()
		
public:		
	// Sets default values for this component's properties
	UInventoryController();
	//UInventoryController(const UInventoryController& other);
	UFUNCTION(BlueprintCallable)
	void CopyInventory(const UInventoryController* other);
	void InitInventoryWidget();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "MyUI")
	void ShowInventory();
	UFUNCTION(BlueprintCallable, Category = "MyUI")
	void CloseInventory();

	UFUNCTION(BlueprintCallable)
	void AddToInventory(ItemType itemType, int count);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "MyUI")
	void UseItem(ItemType itemType);

	UFUNCTION(NetMulticast, Reliable)
	void UseItem_Multicast(ItemType itemType);

	UFUNCTION(BlueprintCallable)
	void ChangeMyGold(int addGold);

	void HandleOpenShop();
	void HandleCloseShop();

	int32 GetGold() { return MyGold; }

	UPROPERTY(BlueprintReadWrite)
	class UInventoryWidget* InventoryWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isShowInventory;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 MyGold;

	UPROPERTY(BlueprintReadWrite)
	class AMyPlayerController* MyOwner;
	
	UPROPERTY(BlueprintReadWrite)
	ItemType WearingWeapon;
	
	UPROPERTY(BlueprintReadWrite)
	class UInventorySlot* CurQuickSlotCandidate;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> UiInventoryBPClass;

	UPROPERTY(EditAnywhere)
	UDataTable* ItemDataTable;
private:
	int32 GetItemIDFromType(ItemType ItemType);
	void RefreshInventorySlots();

	UPROPERTY(EditAnywhere)
	TMap<ItemType, int32>  InvenItemMap;
};
