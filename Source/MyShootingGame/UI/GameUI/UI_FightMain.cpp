// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/UI/GameUI/UI_FightMain.h"
#include "MyShootingGame/Character/PlayerCharacter.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "MyShootingGame/Character/CharacterAtt.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/WidgetSwitcher.h"
#include "MyShootingGame/Logic/MSG_EventManager.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "MyShootingGame/Items/WeaponBase.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "InventoryManagement/Utils/Inv_InventoryStatic.h"
#include "InventoryManagement/Widget/Inv_InfoMessage.h"
#include "../../../../../../../Source/Runtime/GameplayTags/Classes/GameplayTagContainer.h"
#include "Items/Inv_InventoryItem.h"
#include "EquipmentManagement/Components/Inv_EquipmentComponent.h"
#include "Items/Fragments/Inv_ItemFragment.h"




void UUI_FightMain::NativeConstruct()
{
	Super::NativeConstruct();

	// 绑定切换武器UI的委托
	UMSG_EventManager::GetEventManagerInstance()->SwitchGunUIDelegate.BindUObject(this, &UUI_FightMain::ChangWeaponInfo);
	UMSG_EventManager::GetEventManagerInstance()->ShowCrossHairDelegate.BindUObject(this, &UUI_FightMain::SetCrossHairVisibility);
	UMSG_EventManager::GetEventManagerInstance()->SetShootModeUIDelegate.BindUObject(this, &UUI_FightMain::UpdateShootModeUI);
	UMSG_EventManager::GetEventManagerInstance()->HeadShotEventDelegate.BindUObject(this, &UUI_FightMain::PlayHeadShotUIAnim);
	UMSG_EventManager::GetEventManagerInstance()->ShowHitFeedbackDelegate.BindUObject(this, &UUI_FightMain::ShowHitFeedbackUI);
	UMSG_EventManager::GetEventManagerInstance()->ShowKillFeedbackDelegate.BindUObject(this, &UUI_FightMain::ShowKillFeedbackUI);
	UMSG_EventManager::GetEventManagerInstance()->PlayerInjuredDelegate.BindUObject(this, &UUI_FightMain::ShowPlayerInjuredUI);
}

void UUI_FightMain::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (APlayerCharacter* MyPlayer = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		float MapValue = UKismetMathLibrary::MapRangeClamped(MyPlayer->CurCrossHairSpread,
			MyPlayer->MinCrossHairSpread,
			MyPlayer->MaxCrossHairSpread,
			0.f,
			MyPlayer->MaxCrossHairSpread * 200.f);

		CrossHairUP->SetRenderTranslation(FVector2D(0, -MapValue));
		CrossHairDown->SetRenderTranslation(FVector2D(0, MapValue));
		CrossHairLeft->SetRenderTranslation(FVector2D(-MapValue,0 ));
		CrossHairRight->SetRenderTranslation(FVector2D(MapValue,0));
	}
}

void UUI_FightMain::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UI_FightMain] NativeOnInitialized: OwningPlayer is nullptr"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[UI_FightMain] NativeOnInitialized: OwningPlayer = %s"), *PC->GetName());

	UInv_InventoryComponent* InventoryComp =
		UInv_InventoryStatic::GetInventoryComponent(PC);

	if (!InventoryComp)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[UI_FightMain] NativeOnInitialized: InventoryComponent not found on PlayerController %s"),
			*PC->GetName());
		return;
	}

	UE_LOG(LogTemp, Log,
		TEXT("[UI_FightMain] NativeOnInitialized: InventoryComponent found (%s)"),
		*InventoryComp->GetName());

	if (!InventoryComp->NoRoom.IsAlreadyBound(this, &UUI_FightMain::OnNoRoom))
	{
		InventoryComp->NoRoom.AddDynamic(this, &UUI_FightMain::OnNoRoom);
		UE_LOG(LogTemp, Log,
			TEXT("[UI_FightMain] NativeOnInitialized: NoRoom delegate bound"));
	}
	else
	{
		UE_LOG(LogTemp, Verbose,
			TEXT("[UI_FightMain] NativeOnInitialized: NoRoom delegate already bound"));
	}
}


void UUI_FightMain::OnNoRoom()
{
	if (!InfoMessageUI)
	{
		UE_LOG(LogTemp, Verbose,
			TEXT("无InfoMessageUI"));
		return;
	}
	InfoMessageUI->SetMessage(FText::FromString("Room Not Enough"));
}

void UUI_FightMain::SetCrossHairVisibility(bool bIsVisible)
{
	if (bIsVisible)
	{
		CrossHairUP->SetVisibility(ESlateVisibility::Visible);
		CrossHairDown->SetVisibility(ESlateVisibility::Visible);
		CrossHairLeft->SetVisibility(ESlateVisibility::Visible);
		CrossHairRight->SetVisibility(ESlateVisibility::Visible);
		//CrossHairCenter->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CrossHairUP->SetVisibility(ESlateVisibility::Hidden);
		CrossHairDown->SetVisibility(ESlateVisibility::Hidden);
		CrossHairLeft->SetVisibility(ESlateVisibility::Hidden);
		CrossHairRight->SetVisibility(ESlateVisibility::Hidden);
		//CrossHairCenter->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UUI_FightMain::ChangWeaponInfo()
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!Player) return;

	UInv_EquipmentComponent* EquipComp = Player->GetEquipmentComponent();
	if (!EquipComp)
	{
		WeaponInfoSwitcher->SetActiveWidgetIndex(0);
		return;
	}
	
	UInv_InventoryItem* Item = EquipComp->GetActiveWeaponItem();
	if (!IsValid(Item))
	{
		// 没有激活武器 -> 当近战/空手处理
		WeaponInfoSwitcher->SetActiveWidgetIndex(0);
		return;
	}

	const FInv_ItemManifest& Manifest = Item->GetItemManifest();

	// 1) 用装备类型 Tag 决定武器大类
	const FInv_EquipmentFragment* EquipFrag = Manifest.GetFragmentOfType<FInv_EquipmentFragment>();
	if (!EquipFrag)
	{
		WeaponInfoSwitcher->SetActiveWidgetIndex(0);
		return;
	}

	const FGameplayTag EquipType = EquipFrag->GetEquipmentType();

	// 你自己的 tag 路径按实际改：下面给的是你之前用过的 RifleTag
	static const FGameplayTag RifleTag = FGameplayTag::RequestGameplayTag(TEXT("GameItems.Equipment.Weapons.Rifle"));
	static const FGameplayTag MeleeTag = FGameplayTag::RequestGameplayTag(TEXT("GameItems.Equipment.Weapons.Melee"));
	//static const FGameplayTag PistolTag = FGameplayTag::RequestGameplayTag(TEXT("GameItems.Equipment.Weapons.Pistol"));
	//static const FGameplayTag MachineGunTag = FGameplayTag::RequestGameplayTag(TEXT("GameItems.Equipment.Weapons.MachineGun"));
	//static const FGameplayTag ThrowableTag = FGameplayTag::RequestGameplayTag(TEXT("GameItems.Equipment.Weapons.Throwable"));

	// 2) 从“UI 信息 fragment”取 Icon
	// 你之前说过：用于 UI 获取信息的 fragment 是 FInv_InventoryItemFragment
	const FInv_ImageFragment* UIFrag = Manifest.GetFragmentOfType<FInv_ImageFragment>();

	// ⚠️ 这里 Icon 的字段/函数名按你自己的 fragment 改：
	// 例如 UIFrag->Icon / UIFrag->GetIcon() / UIFrag->WeaponIcon 等
	UTexture2D* Icon = nullptr;
	if (UIFrag)
	{
		Icon = UIFrag->GetIcon(); // <-- 按你实际字段名修改
	}

	// 3) 切换 UI
	if (EquipType.MatchesTag(MeleeTag))
	{
		WeaponInfoSwitcher->SetActiveWidgetIndex(0);
		return;
	}

	if (EquipType.MatchesTag(RifleTag))
	{
		if (Icon) WeaponIcon->SetBrushFromTexture(Icon);
		WeaponInfoSwitcher->SetActiveWidgetIndex(1);
		return;
	}

	//if (EquipType.MatchesTag(PistolTag))
	//{
	//	if (Icon) WeaponIcon->SetBrushFromTexture(Icon);
	//	WeaponInfoSwitcher->SetActiveWidgetIndex(1);
	//	return;
	//}

	//if (EquipType.MatchesTag(MachineGunTag))
	//{
	//	if (Icon) WeaponIcon->SetBrushFromTexture(Icon);
	//	WeaponInfoSwitcher->SetActiveWidgetIndex(1);
	//	return;
	//}

	//if (EquipType.MatchesTag(ThrowableTag))
	//{
	//	// 你如果 throwable UI 单独一套，就 SetActiveWidgetIndex(2) 之类
	//	if (Icon) WeaponIcon->SetBrushFromTexture(Icon);
	//	WeaponInfoSwitcher->SetActiveWidgetIndex(1);
	//	return;
	//}

	// 未识别：默认当空手
	WeaponInfoSwitcher->SetActiveWidgetIndex(0);
}


