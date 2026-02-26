// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Widget/Inv_SpatialInventory.h"
#include "Components/Button.h"
#include "Inventory.h"
#include "InventoryManagement/Widget/Inv_InventoryGrid.h"
#include "Items/Component/Inv_ItemComponent.h"
#include "InventoryManagement/Utils/Inv_InventoryStatic.h"
#include "InventoryManagement/Widget/ItemDescription/Inv_ItemDescription.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "InventoryManagement/Widget/GridSlots/Inv_EquippedGrid.h"
#include "InventoryManagement/Widget/HoverItem/Inv_HoverItem.h"
#include "InventoryManagement/Widget/SlottedItems/Inv_EquipSlottedItem.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"




void UInv_SpatialInventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Btn_Equipment->OnClicked.AddDynamic(this, &UInv_SpatialInventory::ShowEquipment);
	Btn_Consumable->OnClicked.AddDynamic(this, &UInv_SpatialInventory::ShowConsumable);
	Btn_MISC->OnClicked.AddDynamic(this, &UInv_SpatialInventory::ShowMISC);

	Grid_Equipment->SetOwningCanvas(CanvasPanel);
	Grid_Consumables->SetOwningCanvas(CanvasPanel);
	Grid_Misc->SetOwningCanvas(CanvasPanel);

	ShowEquipment();

	WidgetTree->ForEachWidget([this](UWidget* Widget)
		{
			UInv_EquippedGrid* EquippedGridSlot = Cast<UInv_EquippedGrid>(Widget);
			if (IsValid(EquippedGridSlot))
			{
				EquippedGridSlots.Add(EquippedGridSlot);
				EquippedGridSlot->OnEquippedGridClicked.AddDynamic(this, &UInv_SpatialInventory::OnEquippedGridClicked);
			}
		});
}

void UInv_SpatialInventory::OnEquippedGridClicked(UInv_EquippedGrid* EquippedGrid, const FGameplayTag& EquipmentTypeTag)
{
	//检查是否可以装备
	if (!CanEquipHoverItem(EquippedGrid, EquipmentTypeTag)) return;

	//创建槽内装备实例，然后添加到槽内
	const float TileSize = UInv_InventoryStatic::GetInventoryWidget(GetOwningPlayer())->GetTileSize();
	UInv_HoverItem* HoverItem = GetHoverItem();
	UInv_EquipSlottedItem* EquippedSlotItem = EquippedGrid->OnItemEquiped(HoverItem->GetInventoryItem(),EquipmentTypeTag,TileSize);
	EquippedSlotItem->OnEquippedGridSlotClicked.AddDynamic(this, &UInv_SpatialInventory::EquippedSlottedItemClicked);
	
	//通知服务器装备操作
	UInv_InventoryComponent* IC = UInv_InventoryStatic::GetInventoryComponent(GetOwningPlayer());
	check(IsValid(IC));

	IC->Server_EquipSlotClicked(HoverItem->GetInventoryItem(), nullptr);

	if (GetOwningPlayer()->GetNetMode() != NM_DedicatedServer)
	{
		IC->OnItemEquip.Broadcast(HoverItem->GetInventoryItem());
	}
	
	//清除悬停物品
	Grid_Equipment->ClearHoverItem();
}

void UInv_SpatialInventory::EquippedSlottedItemClicked(UInv_EquipSlottedItem* SlottedItem)
{
	// 关闭 / 清理物品描述 UI（避免 UI 悬浮态残留）
	UInv_InventoryStatic::ItemUnhovered(GetOwningPlayer());

	// HoverItem 是可堆叠物品时，不允许走“装备交换”逻辑（避免 stack 物品被当装备处理）
	if (IsValid(GetHoverItem()) && GetHoverItem()->IsStackable()) return;

	// 当前“手里拿着要装备的物品”（可能为空：表示只卸载不装备）
	UInv_InventoryItem* ItemToEquip = IsValid(GetHoverItem()) ? GetHoverItem()->GetInventoryItem() : nullptr;

	// 当前 Slot 上已装备的物品（一定是要卸载的对象）
	UInv_InventoryItem* ItemToUnequip = SlottedItem ? SlottedItem->GetInventoryItem() : nullptr;
	if (!IsValid(ItemToUnequip)) return;

	// 找到该已装备物品所在的 EquippedGrid Slot（用于清理逻辑引用 + 复用 Slot）
	UInv_EquippedGrid* EquippedGridSlot = FindSlotWithEquippedItem(ItemToUnequip);
	if (!IsValid(EquippedGridSlot)) return;

	// 先把 Slot 上的逻辑引用断开（Item / Widget 双向引用清空，避免悬挂指针/重复绑定）
	ClearSlotOfItem(EquippedGridSlot);

	// 将“卸下来的装备”放到 HoverItem（实现装备交换：卸下 -> 手里）
	Grid_Equipment->AssignHoverItem(ItemToUnequip);

	// 移除旧的装备 UI（解绑委托 + 从父节点移除）
	RemoveEquippedSlottedItem(SlottedItem);

	// 创建新的装备 UI：复用原 Slot，装上 ItemToEquip（可能为空：代表只卸载）
	MakeEquippedSlottedItem(SlottedItem, EquippedGridSlot, ItemToEquip);

	// 广播装备/卸载事件（通常会触发服务器装备状态更新、属性刷新等）
	BroadcastSlotClickedDelegates(ItemToEquip, ItemToUnequip);
}

bool UInv_SpatialInventory::CanEquipHoverItem(UInv_EquippedGrid* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag) const
{
	// Slot 必须有效且为空（Slot 内存的是“当前已装备的 Item”）
	if (!IsValid(EquippedGridSlot) || EquippedGridSlot->GetInventoryItem().IsValid()) return false;

	// 必须存在 HoverItem（玩家“手里拿着”的物品）
	UInv_HoverItem* HoverItem = GetHoverItem();
	if (!IsValid(HoverItem)) return false;

	UInv_InventoryItem* HoldItem = HoverItem->GetInventoryItem();

	// 装备条件：
	// 1) 有 HoverItem 且非空
	// 2) 不能是可堆叠物品（stackables 走别的逻辑：合并/拆分）
	// 3) 物品分类必须为 Equipment
	// 4) 物品类型 Tag 必须匹配该装备槽的 EquipmentTypeTag（例如 Helmet / Weapon / Armor）
	return HasHoverItem()
		&& IsValid(HoldItem)
		&& !HoverItem->IsStackable()
		&& HoldItem->GetItemManifest().GetItemCategory() == EInv_ItemCategory::Equipment
		&& HoldItem->GetItemManifest().GetItemType().MatchesTag(EquipmentTypeTag);
}

UInv_EquippedGrid* UInv_SpatialInventory::FindSlotWithEquippedItem(UInv_InventoryItem* Item) const
{
	// 注意：EquippedGridSlots 的元素类型是 UInv_EquippedGrid*
	// TArray::FindByPredicate 返回的是 “指向元素的指针”，即 UInv_EquippedGrid**
	auto* FoundEquipedGridSlot = EquippedGridSlots.FindByPredicate([Item](UInv_EquippedGrid* GridSlot)
		{
			return GridSlot->GetInventoryItem() == Item;
		});

	// 解引用一次，拿到数组里存的那个 UInv_EquippedGrid*
	return FoundEquipedGridSlot ? *FoundEquipedGridSlot : nullptr;
}

void UInv_SpatialInventory::ClearSlotOfItem(UInv_EquippedGrid* GridSlot)
{
	if (IsValid(GridSlot))
	{
		// 清理 Slot -> Item / Slot -> Widget 的引用（避免悬挂引用/重复 UI 状态）
		GridSlot->SetInventoryItem(nullptr);
		GridSlot->SetEquippedSlottedItem(nullptr);
	}
}

void UInv_SpatialInventory::RemoveEquippedSlottedItem(UInv_EquipSlottedItem* Item)
{
	if (!IsValid(Item)) return;

	// 如果已绑定回调，先解绑（避免重复触发 / 悬挂绑定）
	if (Item->OnEquippedGridSlotClicked.IsAlreadyBound(this, &ThisClass::EquippedSlottedItemClicked))
	{
		Item->OnEquippedGridSlotClicked.RemoveDynamic(this, &ThisClass::EquippedSlottedItemClicked);
	}

	// 从 UI 层级移除（对象销毁由 GC/生命周期管理决定）
	Item->RemoveFromParent();
}

void UInv_SpatialInventory::MakeEquippedSlottedItem(UInv_EquipSlottedItem* Equipped, UInv_EquippedGrid* GridSlot, UInv_InventoryItem* Item)
{
	if (!IsValid(GridSlot)) return;

	// 在指定 Slot 上装备 Item，并生成对应的 SlottedItem Widget（tileSize 用于格子布局/缩放）
	UInv_EquipSlottedItem* SlottedItem =
		GridSlot->OnItemEquiped(Item, Equipped->GetEquipmentTypeTag(),
			UInv_InventoryStatic::GetInventoryWidget(GetOwningPlayer())->GetTileSize());

	// 绑定点击回调：点击装备槽 -> 触发交换/卸载逻辑
	if (IsValid(SlottedItem))
	{
		SlottedItem->OnEquippedGridSlotClicked.AddDynamic(this, &ThisClass::EquippedSlottedItemClicked);
	}

	// Slot 记录当前 SlottedItem（形成 Slot <-> Widget 的逻辑关联）
	GridSlot->SetEquippedSlottedItem(SlottedItem); 
}

void UInv_SpatialInventory::BroadcastSlotClickedDelegates(UInv_InventoryItem* ItemToEquip, UInv_InventoryItem* ItemToUnequip) const
{
	// 通过 InventoryComponent 走服务器 RPC，同步装备状态（权威端更新 + Replication）
	UInv_InventoryComponent* IC = UInv_InventoryStatic::GetInventoryComponent(GetOwningPlayer());
	check(IsValid(IC));
	IC->Server_EquipSlotClicked(ItemToEquip, ItemToUnequip);

	if (GetOwningPlayer()->GetNetMode() != NM_DedicatedServer)
	{
		IC->OnItemEquip.Broadcast(ItemToEquip);
		IC->OnItemUnequip.Broadcast(ItemToUnequip);
	}
}

void UInv_SpatialInventory::NativeTick(const FGeometry& Geometry, float DeltaTime)
{
	Super::NativeTick(Geometry, DeltaTime);

	// 物品描述框存在时，Tick 里仅做位置/尺寸跟随（例如跟鼠标、跟 Canvas 边界约束）
	if (!IsValid(ItemDescription)) return;
	SetItemDescriptionSizeAndPos(ItemDescription, CanvasPanel);
}

void UInv_SpatialInventory::SetItemDescriptionSizeAndPos(UInv_ItemDescription* Description, UCanvasPanel* Canvas) const
{
	UCanvasPanelSlot* ItemDescriptionCanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Description);
	if (!IsValid(ItemDescriptionCanvasSlot)) return;

	const FVector2D DescriptionSize = Description->GetBoxSize();
	ItemDescriptionCanvasSlot->SetSize(DescriptionSize);

	FVector2D ClampedPos = UInv_WidgetUtils::GetClampedWidgetPosition(UInv_WidgetUtils::GetWidgetSize(Canvas), DescriptionSize, UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer()));

	ItemDescriptionCanvasSlot->SetPosition(ClampedPos);
}

FInv_SlotAvailabilityResult UInv_SpatialInventory::HasRoomForItem(UInv_ItemComponent* ItemComp) const
{
	switch(UInv_InventoryStatic::GetItemCategoryFromItemComp(ItemComp))
	{
		case EInv_ItemCategory::Equipment:
			return Grid_Equipment->HasRoomForItem(ItemComp);
		case EInv_ItemCategory::Consumable:
			return Grid_Consumables->HasRoomForItem(ItemComp);
		case EInv_ItemCategory::Misc:
			return Grid_Misc->HasRoomForItem(ItemComp);
		default:
			UE_LOG(LogInventory, Warning, TEXT("Inv_SpatialInventory::HasRoomForItem - Item has no valid category"));
			return FInv_SlotAvailabilityResult();
	}
}


FReply UInv_SpatialInventory::NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	ActiveGrid->DropItem();
	return FReply::Handled();
}


void UInv_SpatialInventory::OnItemHoverd(UInv_InventoryItem* Item)
{
	const auto& Manifest = Item->GetItemManifest();
	UInv_ItemDescription* DescriptionWidget = GetItemDescription();
	DescriptionWidget->SetVisibility(ESlateVisibility::Collapsed);

	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(DescriptionTimer);

	FTimerDelegate DescriptionTimerDelegate;
	DescriptionTimerDelegate.BindLambda([this,&Manifest,DescriptionWidget]()
	{
		Manifest.AssimilateFragments(DescriptionWidget);
		GetItemDescription()->SetVisibility(ESlateVisibility::HitTestInvisible);
		//把清单内容加到描述中


	});

	GetOwningPlayer()->GetWorldTimerManager().SetTimer(DescriptionTimer, DescriptionTimerDelegate, DescriptionTimerDelay, false);

}

void UInv_SpatialInventory::OnItemUnhoverd()
{
	GetItemDescription()->SetVisibility(ESlateVisibility::Collapsed);
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(DescriptionTimer);
}


bool UInv_SpatialInventory::HasHoverItem() const
{
	if (Grid_Equipment->HasHoverItem()) return true;
	if (Grid_Consumables->HasHoverItem()) return true;
	if (Grid_Misc->HasHoverItem()) return true;
	return false;
}


UInv_HoverItem* UInv_SpatialInventory::GetHoverItem() const
{
	if (!ActiveGrid.IsValid()) return nullptr;

	return ActiveGrid->GetHoverItem();
}


float UInv_SpatialInventory::GetTileSize() const
{
	return Grid_Equipment->GetTileSize();
}

UInv_ItemDescription* UInv_SpatialInventory::GetItemDescription()
{
	if (!IsValid(ItemDescription))
	{
		ItemDescription = CreateWidget<UInv_ItemDescription>(GetOwningPlayer(), ItemDescrioptionClass);
		CanvasPanel->AddChild(ItemDescription);
	}
	return ItemDescription;
}

void UInv_SpatialInventory::ShowEquipment()
{
	SetActiveGrid(Grid_Equipment, Btn_Equipment);
}

void UInv_SpatialInventory::ShowConsumable()
{
	SetActiveGrid(Grid_Consumables, Btn_Consumable);
}

void UInv_SpatialInventory::ShowMISC()
{
	SetActiveGrid(Grid_Misc, Btn_MISC);
}

void UInv_SpatialInventory::SetActiveGrid(UInv_InventoryGrid* Grid, UButton* Btn)
{
	if (ActiveGrid.IsValid())
	{
		ActiveGrid->OnHide();
		ActiveGrid->HideCursor();
	}
	ActiveGrid = Grid;
	if (ActiveGrid.IsValid())ActiveGrid->ShowCursor();
	DisableBtn(Btn);
	Switcher->SetActiveWidget(Grid);
}

void UInv_SpatialInventory::DisableBtn(UButton* Btn)
{
	Btn_Equipment->SetIsEnabled(true);
	Btn_Consumable->SetIsEnabled(true);
	Btn_MISC->SetIsEnabled(true);
	Btn->SetIsEnabled(false);
}

