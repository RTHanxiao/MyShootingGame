// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/UI/GameUI/UI_ShowItem.h"
#include "Components/Image.h"
#include "Engine/TextureRenderTarget2D.h"
#include "../../../../../../../Source/Runtime/SlateCore/Public/Styling/SlateBrush.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "MyShootingGame/Items/WeaponBase.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/TextBlock.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/Button.h"
#include "MyShootingGame/GameCore/MSG_PlayerController.h"
#include "MyShootingGame/Character/PlayerCharacter.h"
#include "MyShootingGame/GameCore/MSG_FightHUD.h"
#include "MyShootingGame/Items/Bullets/BulletActor.h"
#include "MyShootingGame/Items/Weapon/Inv_EquipWeaponActor.h"


void UUI_ShowItem::NativeConstruct()
{
	Super::NativeConstruct();
	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UUI_ShowItem::CloseButtonClicked);
	}
}

//void UUI_ShowItem::SetInspectedItemMesh()
//{
//	// 调试：检查 InspectedItemActor 的状态
//	if (!InspectedItemActor)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("InspectedItemActor is NULL!"));
//		return;
//	}
//
//	UE_LOG(LogTemp, Warning, TEXT("InspectedItemActor: %s, Class: %s"),
//		*InspectedItemActor->GetName(),
//		*InspectedItemActor->GetClass()->GetName());
//
//	AActor* Found = UGameplayStatics::GetActorOfClass(GetWorld(), AInspectedItem::StaticClass());
//	if (Found)
//	{
//		InspectedItem = Cast<AInspectedItem>(Found);
//		UE_LOG(LogTemp, Warning, TEXT("Found InspectedItem: %s"), *InspectedItem->GetName());
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Didn't Found InspectedItem"));
//		return;
//	}
//
//	// 调试：检查具体的类型
//	UE_LOG(LogTemp, Warning, TEXT("Trying to cast to different types..."));
//
//	// 处理武器
//	if (AInv_EquipWeaponActor* Weapon = Cast<AInv_EquipWeaponActor>(InspectedItemActor))
//	{
//		UE_LOG(LogTemp, Warning, TEXT("WeaponInspect - Weapon Name: %s"), *Weapon->GetWeaponName().ToString());
//		InspectedItem->ItemSkeletalMesh->SetSkeletalMeshAsset(Weapon->);
//		InspectedItem->ShowSkeletalMesh();
//		ItemNameText->SetText(FText::FromName(Weapon->GetWeaponName()));
//		ItemDescription->SetText(Weapon->GetWeaponDescription());
//	}
//	// 处理子弹
//	else if (ABulletActor* Bullet = Cast<ABulletActor>(InspectedItemActor))
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Bullet Inspect"));
//		if (Bullet->GetStaticMeshComponent() && Bullet->GetStaticMeshComponent()->GetStaticMesh())
//		{
//			InspectedItem->ItemStaticMesh->SetStaticMesh(Bullet->GetStaticMeshComponent()->GetStaticMesh());
//			InspectedItem->ShowStaticMesh();
//			ItemNameText->SetText(FText::FromString(TEXT("Bullet")));
//			ItemDescription->SetText(FText::FromString(TEXT("Bullet")));
//		}
//		else
//		{
//			UE_LOG(LogTemp, Warning, TEXT("Bullet mesh components are invalid!"));
//		}
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("InspectedItemActor is not a Weapon or Bullet! Actor Class: %s"),
//			*InspectedItemActor->GetClass()->GetName());
//	}
//}

void UUI_ShowItem::CloseButtonClicked()
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
	{
		if(AMSG_PlayerController* PC = Cast<AMSG_PlayerController>(Player->GetController()))
		{
			if(AMSG_FightHUD* HUD = Cast<AMSG_FightHUD>(PC->GetHUD()))
			{
				HUD->CloseShowItemUI();
			}
		}
	}
	InspectedItemActor = nullptr;
}
